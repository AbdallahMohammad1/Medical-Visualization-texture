import vtk

class DICOM_Renderer():
    def __init__(self, dir):
        self.dir = dir
        self.volume = None

    def get_volume(self):
        # The following reader is used to read a series of 2D slices (images)
        # that compose the volume. The slice dimensions are set, and the
        # pixel spacing. The data Endianness must also be specified. The reader
        # usese the FilePrefix in combination with the slice number to construct
        # filenames using the format FilePrefix.%d. (In this case the FilePrefix
        # is the root name of the file: quarter.)
        if self.volume is None:
            self.volume = vtk.vtkDICOMImageReader()
            self.volume.SetDataByteOrderToLittleEndian()
            self.volume.SetDirectoryName(self.dir)
            spc = self.volume.GetDataSpacing()
            self.volume.SetDataSpacing(spc[0], spc[1], spc[2])
        return self.volume

    def surface_render(self,iso):
        volume = self.get_volume()
        # Create the renderer, the render window, and the interactor. The
        # renderer draws into the render window, the interactor enables mouse-
        # and keyboard-based interaction with the scene.
        aRenderer = vtk.vtkRenderer()
        renWin = vtk.vtkRenderWindow()
        renWin.AddRenderer(aRenderer)
        iren = vtk.vtkRenderWindowInteractor()
        iren.SetRenderWindow(renWin)

        # An isosurface, or contour value of 500 is known to correspond to the
        # skin of the patient. Once generated, a vtkPolyDataNormals filter is
        # is used to create normals for smooth surface shading during rendering.
        # The triangle stripper is used to create triangle strips from the
        # isosurface these render much faster on may systems.
        skinExtractor = vtk.vtkContourFilter()
        skinExtractor.SetInputConnection(volume.GetOutputPort())
        skinExtractor.SetValue(0, iso)
        skinNormals = vtk.vtkPolyDataNormals()
        skinNormals.SetInputConnection(skinExtractor.GetOutputPort())
        skinNormals.SetFeatureAngle(60.0)
        skinMapper = vtk.vtkPolyDataMapper()
        skinMapper.SetInputConnection(skinNormals.GetOutputPort())
        skinMapper.ScalarVisibilityOff()
        skin = vtk.vtkActor()
        skin.SetMapper(skinMapper)

        # It is convenient to create an initial view of the data. The FocalPoint
        # and Position form a vector direction. Later on (ResetCamera() method)
        # this vector is used to position the camera to look at the data in
        # this direction.
        aCamera = vtk.vtkCamera()
        aCamera.SetViewUp(0, 0, -1)
        aCamera.SetPosition(0, 1, 0)
        aCamera.SetFocalPoint(0, 0, 0)
        aCamera.ComputeViewPlaneNormal()

        # Actors are added to the renderer. An initial camera view is created.
        # The Dolly() method moves the camera towards the FocalPoint,
        # thereby enlarging the image.
        aRenderer.AddActor(skin)
        aRenderer.SetActiveCamera(aCamera)
        aRenderer.ResetCamera()
        aCamera.Dolly(1)

        # Set a background color for the renderer and set the size of the
        # render window (expressed in pixels).
        aRenderer.SetBackground(0, 0, 0)
        renWin.SetSize(640, 480)

        # Note that when camera movement occurs (as it does in the Dolly()
        # method), the clipping planes often need adjusting. Clipping planes
        # consist of two planes: near and far along the view direction. The
        # near plane clips out objects in front of the plane the far plane
        # clips out objects behind the plane. This way only what is drawn
        # between the planes is actually rendered.
        aRenderer.ResetCameraClippingRange()

        # Interact with the data.
        iren.Initialize()
        renWin.Render()
        iren.Start()


    def ray_render(self):
        volume = self.get_volume()

        # Create the renderer, the render window, and the interactor. The renderer
        # draws into the render window, the interactor enables mouse- and
        # keyboard-based interaction with the scene.
        ren = vtk.vtkRenderer()
        renWin = vtk.vtkRenderWindow()
        renWin.AddRenderer(ren)
        iren = vtk.vtkRenderWindowInteractor()
        iren.SetRenderWindow(renWin)

        # The volume will be displayed by ray-cast alpha compositing.
        # A ray-cast mapper is needed to do the ray-casting, and a
        # compositing function is needed to do the compositing along the ray.
        volumeMapper = vtk.vtkGPUVolumeRayCastMapper()
        volumeMapper.SetInputConnection(volume.GetOutputPort())
        volumeMapper.SetBlendModeToComposite()

        # The color transfer function maps voxel intensities to colors.
        # It is modality-specific, and often anatomy-specific as well.
        # The goal is to one color for flesh (between 500 and 1000)
        # and another color for bone (1150 and over).
        volumeColor = vtk.vtkColorTransferFunction()
        volumeColor.AddRGBPoint(0,    0.0, 0.0, 0.0)
        volumeColor.AddRGBPoint(500,  1.0, 0.5, 0.3)
        volumeColor.AddRGBPoint(1000, 1.0, 0.5, 0.3)
        volumeColor.AddRGBPoint(1150, 1.0, 1.0, 0.9)

        # The opacity transfer function is used to control the opacity
        # of different tissue types.
        volumeScalarOpacity = vtk.vtkPiecewiseFunction()
        volumeScalarOpacity.AddPoint(0,    0.00)
        volumeScalarOpacity.AddPoint(500,  0.15)
        volumeScalarOpacity.AddPoint(1000, 0.15)
        volumeScalarOpacity.AddPoint(1150, 0.85)

        # The gradient opacity function is used to decrease the opacity
        # in the "flat" regions of the volume while maintaining the opacity
        # at the boundaries between tissue types.  The gradient is measured
        # as the amount by which the intensity changes over unit distance.
        # For most medical data, the unit distance is 1mm.
        volumeGradientOpacity = vtk.vtkPiecewiseFunction()
        volumeGradientOpacity.AddPoint(0,   0.0)
        volumeGradientOpacity.AddPoint(90,  0.5)
        volumeGradientOpacity.AddPoint(100, 1.0)

        # The VolumeProperty attaches the color and opacity functions to the
        # volume, and sets other volume properties.  The interpolation should
        # be set to linear to do a high-quality rendering.  The ShadeOn option
        # turns on directional lighting, which will usually enhance the
        # appearance of the volume and make it look more "3D".  However,
        # the quality of the shading depends on how accurately the gradient
        # of the volume can be calculated, and for noisy data the gradient
        # estimation will be very poor.  The impact of the shading can be
        # decreased by increasing the Ambient coefficient while decreasing
        # the Diffuse and Specular coefficient.  To increase the impact
        # of shading, decrease the Ambient and increase the Diffuse and Specular.
        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.SetColor(volumeColor)
        volumeProperty.SetScalarOpacity(volumeScalarOpacity)
        volumeProperty.SetGradientOpacity(volumeGradientOpacity)
        volumeProperty.SetInterpolationTypeToLinear()
        volumeProperty.ShadeOn()
        volumeProperty.SetAmbient(0.4)
        volumeProperty.SetDiffuse(0.6)
        volumeProperty.SetSpecular(0.2)

        # The vtkVolume is a vtkProp3D (like a vtkActor) and controls the position
        # and orientation of the volume in world coordinates.
        volume = vtk.vtkVolume()
        volume.SetMapper(volumeMapper)
        volume.SetProperty(volumeProperty)

        # Finally, add the volume to the renderer
        ren.AddViewProp(volume)

        # Set up an initial view of the volume.  The focal point will be the
        # center of the volume, and the camera position will be 400mm to the
        # patient's left (which is our right).
        camera = ren.GetActiveCamera()
        c = volume.GetCenter()
        camera.SetFocalPoint(c[0], c[1], c[2])
        camera.SetPosition(c[0] + 400, c[1], c[2])
        camera.SetViewUp(0, 0, -1)

        # Increase the size of the render window
        renWin.SetSize(640, 480)

        # Interact with the data.
        iren.Initialize()
        renWin.Render()
        iren.Start()
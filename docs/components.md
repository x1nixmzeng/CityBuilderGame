# Components

## Base Component

The base class of all components is the "Component" class.

## Table of components
- [Auto Nav Component](#autonavcomponent)
- [CameraComponent](#cameracomponent)
- [DebugComponent](#debugcomponent)
- [InstancedMeshComponent](#instancedmeshcomponent)
- [LaraComponent](#laracomponent)
- [MeshComponent](#meshcomponent)
- [MetaBlockComponent](#metablockcomponent)
- [NavBlockComponent](#navblockcomponent)
- [RouteComponent](#routecomponent)
- [SwitchBlockComponent](#switchblockcomponent)
- [TriggerComponent](#triggercomponent)
- [TransformationComponent](#transformationcomponent)

## Details
### AutoNavComponent
Stores an array of inputs required to navigate a level to the end.

### CameraComponent
Stores the camera properties

### DebugComponent
Used to hint which components to include for rendering

### InstancedMeshComponent
Allows the same mesh to be rendered multiple times using an array of transformation

### LaraComponent
Stores the properties of the player character (Lara)

### MeshComponent
Stores a reference to a renderable mesh

### MetaBlockComponent
Stores metadata about a given level cell

### NavBlockComponent
Stores metadata used for navigation and triggers

### RouteComponent
Holds the deserialised level data as an object. Only used when switching levels

### SwitchBlockComponent
Stored a named switch and the state

### TriggerComponent
Stores information about world geometry triggers

### TransformationComponent
Stores a transformation used for translation, scale and rotations

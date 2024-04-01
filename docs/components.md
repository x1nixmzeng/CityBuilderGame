# Components

## Base Component

The base class of all components is the "Component" class.

## Table of components
- [Auto Nav Component](#autonavcomponent)
- [BladeComponent](#bladecomponent)
- [CameraComponent](#cameracomponent)
- [DebugComponent](#debugcomponent)
- [InstancedMeshComponent](#instancedmeshcomponent)
- [LaraComponent](#laracomponent)
- [LightComponent](#lightcomponent)
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

### BladeComponent
Stores blade enemy properties

### CameraComponent
Stores the camera properties

### DebugComponent
Used to hint which components to include for rendering

### NoHitTestComponent
Used to hint which components can be ignored by picking

### InstancedMeshComponent
Allows the same mesh to be rendered multiple times using an array of transformation

### LaraComponent
Stores the properties of the player character (Lara)

### LightComponent
Stores the properties of a dynamic list

**Note:** There is a limit of 4 lights enabled in a scene at once

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

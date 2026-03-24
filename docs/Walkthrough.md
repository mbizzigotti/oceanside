# Scene Files
Unfortunately, the JSON parser that we use does not support comments, but it is also case insensitive (by default), so case doesn't matter!

Here is an example scene file with comments for explaination:

```json
{
    "Camera": {
        "type": "Viewer",    // Starting Camera type
                             // ("Viewer", "First_Person")
        "width": 800,        // Screen width
        "height": 600,       // Screen height
        "fov": 45,           // FOV in degrees
        "distance": 10.0,    // Starting distance from center (Viewer)
        "center": [0,0,0],   // Center of rotation (Viewer)
        "position": [0,0,0], // Starting position (First_Person)
    },

    // Every scenes contains a list of objects.
    "Objects": [
        {
            // Every object must have a type.
            "type": "Mesh",

            // The "Mesh" object type must have a "mesh" file to load.
            // NOTE: All filepaths are relative to the scene file loaction.
            "file": "assets/cube.obj"

            // "Transform" sets an objects model matrix
            "transform": [
                // All transforms available:
                {"scale": [0.15, 0.15, 0.15]},
                {"scale": 1.01},
                {"translate": [-3, 0, 0]}

                // You can implement your own transforms
                //  by modifying `src/loaders/transform_loader.cpp`!
            ]
        },
        {
            // Example of what "Rig" could look like
            //  at the end of completing project 3
            "type": "Rig",
            "skeleton":  "assets/wasp.skel",
            "skin":      "assets/wasp.skin",
            "animation": "assets/wasp.anim",
        }
    ]
}
```

See [Scenes/](../scenes/) for more examples.

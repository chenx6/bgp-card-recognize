# bgp-card-recognize Document

## Exposed function

```cpp
/// @brief process the screenshot and return the result
/// @param offset a pointer points to raw image data
/// @param width screenshot width
/// @param height screenshot height
/// @param hash_data_s bestdori's card hash data
/// @param first_x a dot in left-top card
/// @param first_y a dot in left-top card
/// @return recognized card id that store in vector
vector<int> process(int offset, int width, int height, string hash_data_s,
                    int first_x, int first_y)
```

## How to call this function in javascript

```javascript
// We should draw screenshot into canvas and image
// In this example code, we draw screenshot into #data-canvas

// Get canvas data
let image = document.getElementById('show-img');
let dataCanvas = document.getElementById("data-canvas");
let ctx = dataCanvas.getContext('2d');
let imageData = ctx.getImageData(0, 0, image.width, image.height);
let uintArray = imageData.data;
// Put data into heap
const uint8tPtr = Module._malloc(uintArray.length);
Module.HEAPU8.set(uintArray, uint8tPtr);
let v = Module.process(uint8tPtr, image.width, image.height, hashes, x, y);
Module._free(uint8tPtr);
// Put result into array for further process
let result = [];
for (let i = 0; i < v.size(); i++) {
    result.push(v.get(i));
}
// The return value is stored in `result` array
// Put "<script src="card_recognize.js"></script>" in the html for loading module
```
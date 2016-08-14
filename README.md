What is sdviz ?
===============
sdviz stands for Simple Data Visualizer.  
I developed this library to show image, chart and text data and interact to them by C++.  
sdviz contains http and websocket server. So users can access these data using Web browser.

Installation
===============

    git clone git@github.com:ar90n/sdviz.git
    mkdir build
    cd build
    cmake ../sdviz
    make && make install

Screenshot
===============
![screenshot](https://raw.githubusercontent.com/ar90n/sdviz/images/screenshot.png)

Feature
===============
* Watch data using Web browser via http and websocket.
* Visualize text, image and chart data.
* Support Line, Bar and Scatter Chart
* Support RGB888 Image.
* Support two intaractive components ( Button and Slider ).

Future Feature
===============
* Replace JSON with MessagePack.
* Compress serialized data using LZ4.
* Use [CornerStone](https://github.com/chafey/cornerstone) as image data viewer.
* Add test cases.
* Add excamples.

License
===============
This software is released under the MIT License, see LICENSE.txt.

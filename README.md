# Task 1

* Understand the OSM data model

# Task 2

* Using Osmium Library
* PBF Reader
  * Choose only relative entities
  * Filter out the entities
  * Loop over the entities
* Merge touching coastlines
  * front to back or back to front only

* Speed up by removing closed polygons
* Order them and maybe use a Hashmap
* Use -O3 optimisation flag

* Display a single way

- [X] Extract coastline Ways (first loop)
- [X] Extract nodes ids from ways
- [X] Extract coastline Nodes (second loop)
- [X] Write Geojson file
- [X] Write final method
- [X] Use Hashsets instead of Vectors (Complexity O(1) instead of O(n))
- [X] Antarctica: ~1.5 seconds for 5,101,184 nodes (672,867 coastline nodes)
- [X] Planet: <1.5 minutes for 62,612,921 nodes (55,420,760 coastline nodes)
- [X] Check number of stitched ways after using hashmaps
- [X] Check if the ways are stitched correctly
- [X] Merge all possible ways together

# Task 3

* Differentaite between ocean and land
  * parity of crossing lines
  * Lines 180
  * Grid

  * [X] Bounding Box

    * **Ray casting algorithm** (see Jordan curve theorem):
    * Bounding box for all closed polygons
    * check bounding boxes that the test point are located inside
    * If test ray intersects the bounding box, then check if the point is on a vertex
      * if not on a vertex: compare the ray to each edge of the polygon
        * if ends of an edge are both on the same side of the ray (Northern Hemisphere) and the ray is between the ends of the edge, then the ray intersects the edge

[X] Memory very large (laptop is 64 bit but still the code crashes and is killed for planet.osm.pbf)
  * Use floats instead of doubles
    * No difference
  * Resize and shrink_to_size
    * Not working
  * [X] Clear temporary stitching Hashmap, node_ids, and ways_collection
    * works!!
    * But sometimes VSCode is killed
    * Fresh restart of laptop seems to remove the memory issue

* [X] Check [https://github.com/tylerjereddy/grid-point-sphere] (Li, 2017) paper implementation

Bresenham to list edges that pass by a grid cell
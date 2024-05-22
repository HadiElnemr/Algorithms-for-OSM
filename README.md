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

* Display a single way

- [X] Extract coastline Ways (first loop)
- [X] Extract nodes ids from ways
- [X] Extract coastline Nodes (second loop)
- [X] Write Geojson file
- [X] Write final method
- [X] Use Hashsets instead of Vectors (Complexity O(1) instead of O(n))
- [X] Antarctica: ~1.5 seconds for 5,101,184 nodes (672,867 coastline nodes)
- [X] Planet: <1.5 minutes for 62,612,921 nodes (55,420,760 coastline nodes)
- [ ] Check number of stitched ways after using hashmaps
- [ ] Check if the ways are stitched correctly

# Task 3

* Differentaite between ocean and land

  * Bounding Box

    * **Ray casting algorithm** (see Jordan curve theorem):
    * Bounding box for all closed polygons
    * check bounding boxes that the test point are located inside
    * If test ray intersects the bounding box, then check if the point is on a vertex
      * if not on a vertex: compare the ray to each edge of the polygon
        * if ends of an edge are both on the same side of the ray (Northern Hemisphere) and the ray is between the ends of the edge, then the ray intersects the edge


Bresenham to list edges that pass by a grid cell
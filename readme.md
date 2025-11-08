# VecDB

This is an attempt to implement a simple vector database from scratch. Main goal is to learn how databases work + practice / refresh cpp.

## Storage

### Page structure

Data in DBs is stored in small chunks called pages. Each page is saved on disk as a file and can be loaded on demand to retrieve or push data. To store the data efficiently, each page follows a specific format:
* Page Header:
    * Page ID (4 bytes) - unique ID of the page 
    * Dimensionality (4 bytes) - dimensionality of all vectors stored on the page - a page always stores vectors of the same dimensionality 
    * Vector count (4 bytes) - number of vectors currently stored in the page
* Metadata - for each vector:
    * Vector ID (8 bytes) - unique ID of the vector
    * Offset (4 bytes) - offset of where the vector data starts.
* Data - for each vector:
    * Vector data (dimensionality * sizeof(float) bytes)
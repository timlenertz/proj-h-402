dypc
====

Tim Lenertz

ULB INFO MA1

PROJ-H-402 Project


Des fichiers nuage de points produits par des scanneurs 3D actuels peuvent aisément devenir trop lourds pour pouvoir être traités ou visualisés en entier. Ce projet met en comparaison plusieurs structures de données, formats de fichier et techniques de filtrage qui pourraient permettre d'en extraire dynamiquement des sous-ensembles rélévants. Sur cette base une application a été développée qui créé une telle structure et permet à l'utilisateur de se déplacer à travers et d'explorer un nuage de points de grande taille.

Point cloud data produced by current 3D scanning technology can quickly become too large in size to be processed or visualized as a whole. This project compares several data structures, file formats and filtering techniques that would make it possible to dynamically extract relevant subsets. Based on this an application was developped which creates such a structure and allows the user to seamlessly move through and explore areas of a large point cloud. 


Dependencies:
- GCC Compiler with full C++11 support
- OpenGL + GLEW
- GLM
- wxWidgets 2.8+
- wxformbuilder
- SQLite3
- HDF5 + HDF5 cpp

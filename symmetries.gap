ROT_PERM := PermList([1, 3, 9, 11, 2, 4, 10, 12, 5, 7, 13, 15, 6, 8, 14, 16]);
COMP_PERM := PermList([16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1]);
VERT_FLIP_PERM := PermList([1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16]);
HORIZ_FLIP_PERM := PermList([1, 3, 2, 4, 9, 11, 10, 12, 5, 7, 6, 8, 13, 15, 14, 16]);
g := Group(ROT_PERM, COMP_PERM, VERT_FLIP_PERM, HORIZ_FLIP_PERM);
Orbits(g);
Order(g);
IsAbelian(g);
StructureDescription(g);

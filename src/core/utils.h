#ifndef UTILS_H
#define UTILS_H

#define VEC2(x_, y_) ((Vector2){.x = (x_), .y = (y_)})
#define VEC3(x_, y_, z_) ((Vector3){.x = (x_), .y = (y_), .z = (z_)})
#define VEC4(x_, y_, z_, w_) ((Vector4){.x = (x_), .y = (y_), .z = (z_), .w = (w_)})
#define RECT(x_, y_, w_, h_) ((Rectangle){.x = (x_), .y = (y_), .width = (w_), .height = (h_)})

#define VEC2_ZERO (VEC2(0 , 0))
#define VEC3_ZERO (VEC3(0 , 0, 0))
#define VEC4_ZERO (VEC4(0 , 0, 0, 0))
#define RECT_ZERO (RECT(0 , 0, 0, 0))

#endif // !UTILS_H

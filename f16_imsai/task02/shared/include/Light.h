#ifndef LIGHT_H_
#define LIGHT_H_

typedef enum
{
    Red,
    Amber,
    Green,
    Blink_Green
} Light;

inline void updateLight(const Light light);


#endif // LIGHT_H_

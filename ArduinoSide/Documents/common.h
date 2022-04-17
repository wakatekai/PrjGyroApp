/*****************************************************************************/
/* File Name : common.h                                                      */
/*****************************************************************************/
/*　インクルードガード */
#ifndef COMMON_H
#define COMMON_H

typedef enum {
    FALSE = 0,
    TRUE
} BOOL;

#define OFF     (0)
#define ON      (1)

#define OK      (0)
#define NG      (1)

typedef char            s8;
typedef int             s16; // Arduinoではintは2byte
typedef long            s32;
typedef unsigned char   u8;
typedef unsigned int    u16;
typedef unsigned long   u32;
typedef float           f32;
typedef double          f64;

#endif  /* COMMON_H */
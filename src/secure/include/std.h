/// @file std.h
/// @brief standard macros
///
/// This file is automatically included in _every_ *.c
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-12-14

#ifndef __STD_H
#define __STD_H

// standard booleans
#define     FALSE   0
#define     TRUE    1

// function short-cuts
#define     TO      this_object
#define     PO(x)   previous_object(x)
#define     TI      this_interactive
#define     TP(x)   this_player(x)

// paths
#define     TMPDIR  "/tmp"

#endif // __STD_H

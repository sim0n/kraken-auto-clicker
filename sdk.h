#pragma once

#define UP WM_LBUTTONUP
#define DOWN WM_LBUTTONDOWN

enum click_type_t
{
    butterfly,
    jitter,
    generated
};

enum clicker_mode_t
{
	recorded,
    random
};
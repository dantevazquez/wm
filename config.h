#ifndef CONFIG_H
#define CONFIG_H

#include <X11/X.h>

// General Settings
#define MAX_WINDOWS 9
#define BAR_HEIGHT 24
#define MOD_KEY Mod4Mask

// Bar Appearance
#define BAR_COLOR_ACTIVE_FG "#ffffff"
#define BAR_COLOR_ACTIVE_BG "#555555"
#define BAR_COLOR_INACTIVE_FG "#aaaaaa"
#define BAR_COLOR_INACTIVE_BG "-" 

// Icon Configuration
typedef struct {
    const char *name;
    const char *icon;
} AppIcon;

static const char *DEFAULT_ICON_STR = "󰣆";

static const AppIcon APP_ICONS[] = {
    {"firefox", ""},
    {"st", ""},
    {NULL, NULL} // Terminator
};

#endif

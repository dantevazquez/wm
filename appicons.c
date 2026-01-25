#include <string.h>
#include <strings.h>
#include "appicons.h"

typedef struct {
    char *name;
    char *icon;
} AppIcon;

static AppIcon icons []= {
    {"firefox","" },
    {"st","" },
    {NULL, NULL}
};

static const char *DEFAULT_ICON = "󰣆";

const char* get_default_icon(){
    return DEFAULT_ICON;
}

const char* get_icon_by_name(const char *name){

    if(!name) return DEFAULT_ICON;

    for(int i = 0; icons[i].name != NULL; i++){
        if(strcasecmp(name, icons[i].name) == 0){
            return icons[i].icon;
        }
    }
    return DEFAULT_ICON;
}


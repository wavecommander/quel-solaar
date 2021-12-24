//
//  b_macos_main.m
//  BetrayTest
//
//  Created by Kyle Fleming on 5/19/17.
//  Copyright © 2017 Symmetry Labs. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include <objc/message.h>
#include <objc/NSObjCRuntime.h>
#include <mach-o/dyld.h>
#include <OpenGL/gl.h>
#include <dlfcn.h>
#include "betray.h"
#include <sys/syslimits.h>

#import <Cocoa/Cocoa.h>

extern id betray_window;

char b_macos_requester_load[PATH_MAX+1] = {0};
char b_macos_requester_save[PATH_MAX+1] = {0};
void *b_macos_requester_load_id = NULL;
void *b_macos_requester_save_id = NULL;

char *betray_requester_save_get(void *save_id)
{
    if (b_macos_requester_save[0] != '\0' && b_macos_requester_save_id == save_id) {
        char *str = malloc((int)strlen(b_macos_requester_save));
        strcpy(str, b_macos_requester_save);
        b_macos_requester_save[0] = '\0';
        b_macos_requester_save_id = NULL;
    }
    return NULL;
}

void betray_requester_save(char **types, uint type_count, void *save_id)
{
    NSSavePanel *panel = [NSSavePanel savePanel];
    
    if (type_count > 0) {
        NSMutableArray *imageTypes = [[NSMutableArray alloc] initWithCapacity:type_count];
        if (imageTypes) {
            NSInteger count = 0;
            while (count++ < type_count) {
                [imageTypes addObject:[NSString stringWithUTF8String:types[count]]];
            }
        }
        [panel setAllowedFileTypes:imageTypes];
    }
    
    __unsafe_unretained typeof(panel) weakPanel = panel;
    [panel beginSheetModalForWindow:betray_window completionHandler:^(NSInteger result) {
        if (result == NSFileHandlingPanelOKButton) {
            NSURL* fileUrl = [weakPanel URL];
            
            const char* str = [[fileUrl absoluteString] cStringUsingEncoding:NSUTF8StringEncoding];
            strncpy(b_macos_requester_save, str, PATH_MAX);
            b_macos_requester_save[PATH_MAX] = '\0';
            
            b_macos_requester_save_id = save_id;
        }
    }];
}

char *betray_requester_load_get(void *load_id)
{
    if (b_macos_requester_load[0] != '\0' && b_macos_requester_load_id == load_id) {
        char *str = malloc((int)strlen(b_macos_requester_load));
        strcpy(str, b_macos_requester_load);
        b_macos_requester_load[0] = '\0';
        b_macos_requester_load_id = NULL;
    }
    return NULL;
}

void betray_requester_load(char **types, uint type_count, void *load_id)
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    
    if (type_count > 0) {
        NSMutableArray *imageTypes = [[NSMutableArray alloc] initWithCapacity:type_count];
        if (imageTypes) {
            NSInteger count = 0;
            while (count++ < type_count) {
                [imageTypes addObject:[NSString stringWithUTF8String:types[count]]];
            }
        }
        [panel setAllowedFileTypes:imageTypes];
    }
    
    __unsafe_unretained typeof(panel) weakPanel = panel;
    [panel beginSheetModalForWindow:betray_window completionHandler:^(NSInteger result) {
        if (result == NSFileHandlingPanelOKButton) {
            NSURL* fileUrl = [[weakPanel URLs] firstObject];
            
            const char* str = [[fileUrl absoluteString] cStringUsingEncoding:NSUTF8StringEncoding];
            strncpy(b_macos_requester_load, str, PATH_MAX);
            b_macos_requester_load[PATH_MAX] = '\0';
            
            b_macos_requester_load_id = load_id;
        }
        
    }];
}

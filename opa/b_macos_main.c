#ifdef __APPLE_CC__

// this code should work if compiled as C99+ or Objective-C (with or without ARC)

// we don't need much here
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

// maybe this is available somewhere in objc runtime?
#if __LP64__ || (TARGET_OS_EMBEDDED && !TARGET_OS_IPHONE) || TARGET_OS_WIN32 || NS_BUILD_32_LIKE_64
#define NSIntegerEncoding "q"
#define NSUIntegerEncoding "L"
#else
#define NSIntegerEncoding "i"
#define NSUIntegerEncoding "I"
#endif

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#else
// this is how they are defined originally
#include <CoreGraphics/CGBase.h>
#include <CoreGraphics/CGGeometry.h>
typedef CGPoint NSPoint;
typedef CGRect NSRect;

extern id NSApp;
extern id const NSDefaultRunLoopMode;
#endif

#if defined(__OBJC__) && __has_feature(objc_arc)
#define ARC_AVAILABLE
#endif

// ABI is a bit different between platforms
#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#else
#define abi_objc_msgSend_stret objc_msgSend_stret
#endif
#ifdef __i386__
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#else
#define abi_objc_msgSend_fpret objc_msgSend
#endif

bool terminated = false;
uint32_t windowCount = 0;

// we gonna construct objective-c class by hand in runtime, so wow, so hacker!
//@interface AppDelegate : NSObject<NSApplicationDelegate>
//-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
//@end
//@implementation AppDelegate
//-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
//{
//	terminated = true;
//	return NSTerminateCancel;
//}
//@end
NSUInteger applicationShouldTerminate(id self, SEL _sel, id sender)
{
	printf("requested to terminate\n");
	terminated = true;
	return 0;
}

//@interface WindowDelegate : NSObject<NSWindowDelegate>
//-(void)windowWillClose:(NSNotification*)notification;
//@end
//@implementation WindowDelegate
//-(void)windowWillClose:(NSNotification*)notification
//{
//	(void)notification;
//	assert(windowCount);
//	if(--windowCount == 0)
//		terminated = true;
//}
//@end

id betray_opengl_context_default;
id betray_opengl_context_current;
id betray_content_view;
id betray_window;

uint betray_screen_screen_resolution_x = 0;
uint betray_screen_screen_resolution_y = 0;

uint betray_scroll_axis_id = -1;

extern uint betray_plugin_axis_allocate(uint user_id, char *name, BAxisType type, uint axis_count);
extern void betray_plugin_axis_set(uint id, float axis_x, float axis_y, float axis_z);
extern void betray_plugin_callback_main(BInputState *input);
extern void betray_reshape_view(uint x_size, uint y_size);
extern void betray_action(BActionMode mode);
extern void betray_time_update();

void windowWillClose(id self, SEL _sel, id notification)
{
	printf("window will close\n");
	assert(windowCount);
	if(--windowCount == 0)
		terminated = true;
}


//extern boolean b_win32_init_display_opengles2(uint size_x, uint size_y, boolean full_screen, uint samples, char *caption, boolean *sterioscopic);

boolean b_win32_init_display_opengl(uint size_x, uint size_y, boolean full_screen, uint samples, char *caption, boolean *sterioscopic)
{
	SEL allocSel = sel_registerName("alloc");
	SEL initSel = sel_registerName("init");
    void *test = NULL;
	#ifdef ARC_AVAILABLE
	@autoreleasepool
	{
	#else
	//NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	//would be nice to use objc_autoreleasePoolPush instead, but it's not publically available in the headers
	Class NSAutoreleasePoolClass = objc_getClass("NSAutoreleasePool");
	id poolAlloc = ((id (*)(Class, SEL))objc_msgSend)(NSAutoreleasePoolClass, allocSel);
	id pool = ((id (*)(id, SEL))objc_msgSend)(poolAlloc, initSel);
	#endif

	//[NSApplication sharedApplication];
	Class NSApplicationClass = objc_getClass("NSApplication");
	SEL sharedApplicationSel = sel_registerName("sharedApplication");
	((id (*)(Class, SEL))objc_msgSend)(NSApplicationClass, sharedApplicationSel);
        
        


	//[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	SEL setActivationPolicySel = sel_registerName("setActivationPolicy:");
	((void (*)(id, SEL, NSInteger))objc_msgSend)(NSApp, setActivationPolicySel, 0);

	//AppDelegate * dg = [[AppDelegate alloc] init];
	Class NSObjectClass = objc_getClass("NSObject");
    test = NSObjectClass;
	Class AppDelegateClass = objc_allocateClassPair(NSObjectClass, "AppDelegateOR", 0);
    test = AppDelegateClass;
    Protocol* NSApplicationDelegateProtocol = objc_getProtocol("NSApplicationDelegate");
    test = NSApplicationDelegateProtocol;
	bool resultAddProtoc = class_addProtocol(AppDelegateClass, NSApplicationDelegateProtocol);
//	assert(resultAddProtoc);
	SEL applicationShouldTerminateSel = sel_registerName("applicationShouldTerminate:");
	bool resultAddMethod = class_addMethod(AppDelegateClass, applicationShouldTerminateSel, (IMP)applicationShouldTerminate, NSUIntegerEncoding "@:@");
	assert(resultAddMethod);
	id dgAlloc = ((id (*)(Class, SEL))objc_msgSend)(AppDelegateClass, allocSel);
	id dg = ((id (*)(id, SEL))objc_msgSend)(dgAlloc, initSel);

	SEL autoreleaseSel = sel_registerName("autorelease");
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(dg, autoreleaseSel);
	#endif

	//[NSApp setDelegate:dg];
	SEL setDelegateSel = sel_registerName("setDelegate:");
	((void (*)(id, SEL, id))objc_msgSend)(NSApp, setDelegateSel, dg);

	// only needed if we don't use [NSApp run]
	//[NSApp finishLaunching];
	SEL finishLaunchingSel = sel_registerName("finishLaunching");
	((void (*)(id, SEL))objc_msgSend)(NSApp, finishLaunchingSel);

	//id menubar = [[NSMenu alloc] init];
	Class NSMenuClass = objc_getClass("NSMenu");
	id menubarAlloc = ((id (*)(Class, SEL))objc_msgSend)(NSMenuClass, allocSel);
	id menubar = ((id (*)(id, SEL))objc_msgSend)(menubarAlloc, initSel);
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(menubar, autoreleaseSel);
	#endif

	//id appMenuItem = [[NSMenuItem alloc] init];
	Class NSMenuItemClass = objc_getClass("NSMenuItem");
	id appMenuItemAlloc = ((id (*)(Class, SEL))objc_msgSend)(NSMenuItemClass, allocSel);
	id appMenuItem = ((id (*)(id, SEL))objc_msgSend)(appMenuItemAlloc, initSel);
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(appMenuItem, autoreleaseSel);
	#endif

	//[menubar addItem:appMenuItem];
	SEL addItemSel = sel_registerName("addItem:");
	((void (*)(id, SEL, id))objc_msgSend)(menubar, addItemSel, appMenuItem);

	//[NSApp setMainMenu:menubar];
	SEL setMainMenuSel = sel_registerName("setMainMenu:");
	((id (*)(id, SEL, id))objc_msgSend)(NSApp, setMainMenuSel, menubar);

	//id appMenu = [[NSMenu alloc] init];
	id appMenuAlloc = ((id (*)(Class, SEL))objc_msgSend)(NSMenuClass, allocSel);
	id appMenu = ((id (*)(id, SEL))objc_msgSend)(appMenuAlloc, initSel);
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(appMenu, autoreleaseSel);
	#endif

	//id appName = [[NSProcessInfo processInfo] processName];
	Class NSProcessInfoClass = objc_getClass("NSProcessInfo");
	SEL processInfoSel = sel_registerName("processInfo");
	id processInfo = ((id (*)(Class, SEL))objc_msgSend)(NSProcessInfoClass, processInfoSel);
	SEL processNameSel = sel_registerName("processName");
	id appName = ((id (*)(id, SEL))objc_msgSend)(processInfo, processNameSel);

	//id quitTitle = [@"Quit " stringByAppendingString:appName];
	Class NSStringClass = objc_getClass("NSString");
	SEL stringWithUTF8StringSel = sel_registerName("stringWithUTF8String:");
	id quitTitlePrefixString = ((id (*)(Class, SEL, const char*))objc_msgSend)(NSStringClass, stringWithUTF8StringSel, "Quit ");
	SEL stringByAppendingStringSel = sel_registerName("stringByAppendingString:");
	id quitTitle = ((id (*)(id, SEL, id))objc_msgSend)(quitTitlePrefixString, stringByAppendingStringSel, appName);

	//id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];
	id quitMenuItemKey = ((id (*)(Class, SEL, const char*))objc_msgSend)(NSStringClass, stringWithUTF8StringSel, "q");
	id quitMenuItemAlloc = ((id (*)(Class, SEL))objc_msgSend)(NSMenuItemClass, allocSel);
	SEL initWithTitleSel = sel_registerName("initWithTitle:action:keyEquivalent:");
	SEL terminateSel = sel_registerName("terminate:");
	id quitMenuItem = ((id (*)(id, SEL, id, SEL, id))objc_msgSend)(quitMenuItemAlloc, initWithTitleSel, quitTitle, terminateSel, quitMenuItemKey);
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(quitMenuItem, autoreleaseSel);
	#endif

	//[appMenu addItem:quitMenuItem];
	((void (*)(id, SEL, id))objc_msgSend)(appMenu, addItemSel, quitMenuItem);

	//[appMenuItem setSubmenu:appMenu];
	SEL setSubmenuSel = sel_registerName("setSubmenu:");
	((void (*)(id, SEL, id))objc_msgSend)(appMenuItem, setSubmenuSel, appMenu);

    Class NSScreenClass = objc_getClass("NSScreen");
    SEL mainScreensSel = sel_registerName("mainScreen");
    id mainScreen = ((id (*)(Class, SEL))objc_msgSend)(NSScreenClass, mainScreensSel);
    SEL screensSel = sel_registerName("screens");
    id screen_array = ((id (*)(Class, SEL))objc_msgSend)(NSScreenClass, screensSel);
    SEL countSel = sel_registerName("count");
    uint array_length = ((uint (*)(Class, SEL))objc_msgSend)(screen_array, countSel);
   
    SEL objectAtIndexSel = sel_registerName("objectAtIndex:");
    id specific_screen = ((id (*)(Class, SEL, uint))objc_msgSend)(screen_array, objectAtIndexSel, 0);
        
        
    SEL visibleFrameSel = sel_registerName("frame");
    NSRect rect = ((NSRect (*)(id, SEL))abi_objc_msgSend_stret)(specific_screen, visibleFrameSel);
        
    betray_screen_screen_resolution_x = rect.size.width;
    betray_screen_screen_resolution_y = rect.size.height;
 
 //   NSRect adjustFrame = ((NSRect (*)(id, SEL))abi_objc_msgSend_stret)(betray_content_view, frameSel)
	//id window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 500, 500) styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask backing:NSBackingStoreBuffered defer:NO];
	;
    rect.origin.x = 0;
    rect.origin.y = 0;
    if(size_x != 0)
        rect.size.width = size_x;
    if(size_y != 0)
        rect.size.height = size_y;
	Class NSWindowClass = objc_getClass("NSWindow");
	id windowAlloc = ((id (*)(Class, SEL))objc_msgSend)(NSWindowClass, allocSel);
	SEL initWithContentRectSel = sel_registerName("initWithContentRect:styleMask:backing:defer:");
	betray_window = ((id (*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(windowAlloc, initWithContentRectSel, rect, 15, 2, NO);
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(betray_window, autoreleaseSel);
	#endif

	// when we are not using ARC, than window will be added to autorelease pool
	// so if we close it by hand (pressing red button), we don't want it to be released for us
	// so it will be released by autorelease pool later
	//[window setReleasedWhenClosed:NO];
	SEL setReleasedWhenClosedSel = sel_registerName("setReleasedWhenClosed:");
	((void (*)(id, SEL, BOOL))objc_msgSend)(betray_window, setReleasedWhenClosedSel, NO);

	windowCount = 1;

	//WindowDelegate * wdg = [[WindowDelegate alloc] init];
	Class WindowDelegateClass = objc_allocateClassPair(NSObjectClass, "WindowDelegate", 0);
	Protocol* NSWindowDelegateProtocol = objc_getProtocol("NSWindowDelegate");
	resultAddProtoc = class_addProtocol(WindowDelegateClass, NSWindowDelegateProtocol);
	assert(resultAddProtoc);
	SEL windowWillCloseSel = sel_registerName("windowWillClose:");
	resultAddMethod = class_addMethod(WindowDelegateClass, windowWillCloseSel, (IMP)windowWillClose,  "v@:@");
	assert(resultAddMethod);
	id wdgAlloc = ((id (*)(Class, SEL))objc_msgSend)(WindowDelegateClass, allocSel);
	id wdg = ((id (*)(id, SEL))objc_msgSend)(wdgAlloc, initSel);
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(wdg, autoreleaseSel);
	#endif

	//[window setDelegate:wdg];
	((void (*)(id, SEL, id))objc_msgSend)(betray_window, setDelegateSel, wdg);
        
        

  //

	//NSView * contentView = [window contentView];
	SEL contentViewSel = sel_registerName("contentView");
	betray_content_view = ((id (*)(id, SEL))objc_msgSend)(betray_window, contentViewSel);

	// disable this if you don't want retina support
	//[contentView setWantsBestResolutionOpenGLSurface:YES];
	SEL setWantsBestResolutionOpenGLSurfaceSel = sel_registerName("setWantsBestResolutionOpenGLSurface:");
	((void (*)(id, SEL, BOOL))objc_msgSend)(betray_content_view, setWantsBestResolutionOpenGLSurfaceSel, YES);

	//[window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
    NSPoint point = {0, 0};
    if(size_x != 0)
       point.x = (betray_screen_screen_resolution_x - size_x) / 2;
    if(size_y != 0)
       point.y = (betray_screen_screen_resolution_y + size_y) / 2;
	SEL cascadeTopLeftFromPointSel = sel_registerName("cascadeTopLeftFromPoint:");
	((void (*)(id, SEL, NSPoint))objc_msgSend)(betray_window, cascadeTopLeftFromPointSel, point);

	id titleString = ((id (*)(Class, SEL, const char*))objc_msgSend)(NSStringClass, stringWithUTF8StringSel, caption);
	SEL setTitleSel = sel_registerName("setTitle:");
	((void (*)(id, SEL, id))objc_msgSend)(betray_window, setTitleSel, titleString);

	//NSOpenGLPixelFormatAttribute glAttributes[] =
	//{
	//	NSOpenGLPFAColorSize, 24,
	//	NSOpenGLPFAAlphaSize, 8,
	//	NSOpenGLPFADoubleBuffer,
	//	NSOpenGLPFAAccelerated,
	//	NSOpenGLPFANoRecovery,
	//	NSOpenGLPFASampleBuffers, 1,
	//	NSOpenGLPFASamples, 4,
	//	NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy, // or NSOpenGLProfileVersion3_2Core
	//	0
	//};
	uint32_t glAttributes[] =
	{
		8, 24,
		11, 8,
		5,
		73,
		72,
		55, 1,
		56, 4,
		99, 0x1000, // or 0x1000 0x3200
		0
	};
    glAttributes[10] = samples;

	//NSOpenGLPixelFormat * pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:glAttributes];
	Class NSOpenGLPixelFormatClass = objc_getClass("NSOpenGLPixelFormat");
	id pixelFormatAlloc = ((id (*)(Class, SEL))objc_msgSend)(NSOpenGLPixelFormatClass, allocSel);
	SEL initWithAttributesSel = sel_registerName("initWithAttributes:");
	id pixelFormat = ((id (*)(id, SEL, const uint32_t*))objc_msgSend)(pixelFormatAlloc, initWithAttributesSel, glAttributes);
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(pixelFormat, autoreleaseSel);
	#endif

	//NSOpenGLContext * openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
	Class NSOpenGLContextClass = objc_getClass("NSOpenGLContext");
	id openGLContextAlloc = ((id (*)(Class, SEL))objc_msgSend)(NSOpenGLContextClass, allocSel);
	SEL initWithFormatSel = sel_registerName("initWithFormat:shareContext:");
	betray_opengl_context_default = ((id (*)(id, SEL, id, id))objc_msgSend)(openGLContextAlloc, initWithFormatSel, pixelFormat, nil);
    betray_opengl_context_current = betray_opengl_context_default;
	#ifndef ARC_AVAILABLE
	((void (*)(id, SEL))objc_msgSend)(betray_opengl_context_current, autoreleaseSel);
	#endif

	//[openGLContext setView:contentView];
	SEL setViewSel = sel_registerName("setView:");
	((void (*)(id, SEL, id))objc_msgSend)(betray_opengl_context_current, setViewSel, betray_content_view);

	//[window makeKeyAndOrderFront:window];
	SEL makeKeyAndOrderFrontSel = sel_registerName("makeKeyAndOrderFront:");
	((void (*)(id, SEL, id))objc_msgSend)(betray_window, makeKeyAndOrderFrontSel, betray_window);

	//[window setAcceptsMouseMovedEvents:YES];
	SEL setAcceptsMouseMovedEventsSel = sel_registerName("setAcceptsMouseMovedEvents:");
	((void (*)(id, SEL, BOOL))objc_msgSend)(betray_window, setAcceptsMouseMovedEventsSel, YES);

	//[window setBackgroundColor:[NSColor blackColor]];
	Class NSColorClass = objc_getClass("NSColor");
	id blackColor = ((id (*)(Class, SEL))objc_msgSend)(NSColorClass, sel_registerName("blackColor"));
	SEL setBackgroundColorSel = sel_registerName("setBackgroundColor:");
	((void (*)(id, SEL, id))objc_msgSend)(betray_window, setBackgroundColorSel, blackColor);

	// TODO do we really need this?
	//[NSApp activateIgnoringOtherApps:YES];
	SEL activateIgnoringOtherAppsSel = sel_registerName("activateIgnoringOtherApps:");
	((void (*)(id, SEL, BOOL))objc_msgSend)(NSApp, activateIgnoringOtherAppsSel, YES);

    if(full_screen)
    {
//        SEL setCollectionBehaviorSel = sel_registerName("setCollectionBehavior");
//        ((void (*)(id, SEL, uint64))objc_msgSend)(betray_window, setCollectionBehaviorSel, 1 << 7); /*NSWindowCollectionBehaviorFullScreenPrimary*/
    }
    SEL makeCurrentContextSel = sel_registerName("makeCurrentContext");
    ((void (*)(id, SEL))objc_msgSend)(betray_opengl_context_current, makeCurrentContextSel);
        
    SEL presentationOptionsSel = sel_registerName("presentationOptions");
    ((id (*)(id, SEL, uint))objc_msgSend)(NSApp, presentationOptionsSel, (1 << 10));
        
    betray_scroll_axis_id = betray_plugin_axis_allocate(0, "Scroll", B_AXIS_SCORLL, 2);

    return TRUE;
}


void betray_launch_main_loop(void)
{
    static uint32 last_modifyer_keys = 0;
    BInputState *input;
    uint i, j, event_timer = 0;
    input = betray_get_input_state();
	printf("entering runloop\n");
	Class NSDateClass = objc_getClass("NSDate");
	SEL distantPastSel = sel_registerName("distantPast");
	SEL nextEventMatchingMaskSel = sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:");
	SEL frameSel = sel_registerName("frame");
	SEL typeSel = sel_registerName("type");
	SEL buttonNumberSel = sel_registerName("buttonNumber");
	SEL keyCodeSel = sel_registerName("keyCode");
	SEL keyWindowSel = sel_registerName("keyWindow");
	SEL mouseLocationOutsideOfEventStreamSel = sel_registerName("mouseLocationOutsideOfEventStream");
	SEL convertRectToBackingSel = sel_registerName("convertRectToBacking:");
	SEL scrollingDeltaXSel = sel_registerName("scrollingDeltaX");
	SEL scrollingDeltaYSel = sel_registerName("scrollingDeltaY");
	SEL hasPreciseScrollingDeltasSel = sel_registerName("hasPreciseScrollingDeltas");
	SEL modifierFlagsSel = sel_registerName("modifierFlags");
	SEL charactersSel = sel_registerName("characters");
	SEL UTF8StringSel = sel_registerName("UTF8String");
	SEL sendEventSel = sel_registerName("sendEvent:");
	SEL updateWindowsSel = sel_registerName("updateWindows");
	SEL updateSel = sel_registerName("update");
	SEL makeCurrentContextSel = sel_registerName("makeCurrentContext");
    SEL flushBufferSel = sel_registerName("flushBuffer");
    SEL contentViewSel = sel_registerName("contentView");

	while(!terminated)
	{
        
		//NSEvent * event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
		id distantPast = ((id (*)(Class, SEL))objc_msgSend)(NSDateClass, distantPastSel);
        id event;

		while((event = ((id (*)(id, SEL, NSUInteger, id, id, BOOL))objc_msgSend)(NSApp, nextEventMatchingMaskSel, NSUIntegerMax, distantPast, NSDefaultRunLoopMode, YES)) != NULL)
		{
            event_timer = 0;
			//NSEventType eventType = [event type];
			NSUInteger eventType = ((NSUInteger (*)(id, SEL))objc_msgSend)(event, typeSel);

			switch(eventType)
			{
				//case NSMouseMoved:
				//case NSLeftMouseDragged:
				//case NSRightMouseDragged:
				//case NSOtherMouseDragged:
				case 5:
				case 6:
				case 7:
				case 27:
				{
					//NSWindow * currentWindow = [NSApp keyWindow];
					id currentWindow = ((id (*)(id, SEL))objc_msgSend)(NSApp, keyWindowSel);

					//NSRect adjustFrame = [[currentWindow contentView] frame];
					id currentWindowContentView = ((id (*)(id, SEL))objc_msgSend)(betray_window, contentViewSel);
					NSRect adjustFrame = ((NSRect (*)(id, SEL))abi_objc_msgSend_stret)(betray_content_view, frameSel);

					//NSPoint p = [currentWindow mouseLocationOutsideOfEventStream];
					// NSPoint is small enough to fit a register, so no need for objc_msgSend_stret
					NSPoint p = ((NSPoint (*)(id, SEL))objc_msgSend)(currentWindow, mouseLocationOutsideOfEventStreamSel);

					// map input to content view rect
					if(p.x < 0)
                        p.x = 0;
					else if(p.x > adjustFrame.size.width)
                        p.x = adjustFrame.size.width;
					if(p.y < 0)
                        p.y = 0;
					else if(p.y > adjustFrame.size.height)
                        p.y = adjustFrame.size.height;
                    input->pointers[0].pointer_x = (float)p.x / (float)adjustFrame.size.width * 2.0 - 1.0;
                    input->pointers[0].pointer_y = (-1.0 + (float)p.y / (float)adjustFrame.size.height * 2.0) * (float)adjustFrame.size.height / (float)adjustFrame.size.width;

					// map input to pixels
					NSRect r = {p.x, p.y, 0, 0};
					//r = [currentWindowContentView convertRectToBacking:r];
					r = ((NSRect (*)(id, SEL, NSRect))abi_objc_msgSend_stret)(currentWindowContentView, convertRectToBackingSel, r);
					p = r.origin;

					printf("mouse moved to %f %f\n", p.x, p.y);
					
				}
                break;
				//case NSLeftMouseDown:
                case 1:
                    input->pointers[0].button[0] = TRUE;
					printf("mouse left key down\n");
				break;
				//case :
				case 2:
                    input->pointers[0].button[0] = FALSE;
					printf("mouse left key up\n");
				break;
				//case NSRightMouseDown:
                case 3:
                    input->pointers[0].button[1] = TRUE;
					printf("mouse right key down\n");
				break;
				//case NSRightMouseUp:
                case 4:
                    input->pointers[0].button[1] = FALSE;
					printf("mouse right key up\n");
				break;
				//case NSOtherMouseDown:
				case 25:
				{
					// number == 2 is a middle button
					//NSInteger number = [event buttonNumber];
                    NSInteger number = ((NSInteger (*)(id, SEL))objc_msgSend)(event, buttonNumberSel);
                    if(number < B_POINTER_BUTTONS_COUNT)
                        input->pointers[0].button[number] = TRUE;
					printf("mouse other key down : %i\n", (int)number);
				}
                break;
				//case NSOtherMouseUp:
				case 26:
				{
					//NSInteger number = [event buttonNumber];
					NSInteger number = ((NSInteger (*)(id, SEL))objc_msgSend)(event, buttonNumberSel);
                    if(number < B_POINTER_BUTTONS_COUNT)
                        input->pointers[0].button[number] = FALSE;
				}
                break;
				//case NSScrollWheel:
				case 22:
				{
                    CGFloat delta_x, delta_y;
                    BOOL precision_scrolling;
					delta_x = ((CGFloat (*)(id, SEL))abi_objc_msgSend_fpret)(event, scrollingDeltaXSel);
					delta_y = ((CGFloat (*)(id, SEL))abi_objc_msgSend_fpret)(event, scrollingDeltaYSel);
					precision_scrolling = ((BOOL (*)(id, SEL))objc_msgSend)(event, hasPreciseScrollingDeltasSel);
					if(precision_scrolling)
					{
						delta_x *= 0.1f; // similar to glfw
						delta_y *= 0.1f;
                    }
                    if(delta_x >= 0.001)
                        betray_plugin_button_set(0, BETRAY_BUTTON_SCROLL_UP, TRUE, -1);
                    if(delta_y <= -0.001)
                        betray_plugin_button_set(0, BETRAY_BUTTON_SCROLL_DOWN, TRUE, -1);
                    if(delta_y >= 0.001)
                        betray_plugin_button_set(0, BETRAY_BUTTON_SCROLL_LEFT, TRUE, -1);
                    if(delta_y <= -0.001)
                        betray_plugin_button_set(0, BETRAY_BUTTON_SCROLL_RIGHT, TRUE, -1);
                    betray_plugin_axis_set(betray_scroll_axis_id, (float)delta_x, (float)delta_y, 0);
				}
                break;
				//case NSFlagsChanged:
				case 12:
				{
                    uint32 bits;
                    NSUInteger modifiers = ((NSUInteger (*)(id, SEL))objc_msgSend)(event, modifierFlagsSel);

                    bits = (modifiers & 0xffff0000UL) >> 16;
                 /*   for(i = 0; i < 32; i++)
                    {
                        printf("key bits %u = %u\n", i, bits % 2);
                        bits /= 2;
                    }*/
// command == 16
                    if((bits & 2) != (last_modifyer_keys & 2))
						betray_plugin_button_set(0, BETRAY_BUTTON_SHIFT, bits & 2, -1);
					if((bits & 4) != (last_modifyer_keys & 4))
						betray_plugin_button_set(0, BETRAY_BUTTON_CONTROL, bits & 4, -1);
					if((bits & 8) != (last_modifyer_keys & 8))
						betray_plugin_button_set(0, BETRAY_BUTTON_ALT, bits & 8, -1);
					last_modifyer_keys = bits;


				//	printf("mod keys : mask %03u state %u%u%u%u%u%u%u%u\n", keys.mask, keys.alpha_shift, keys.shift, keys.control, keys.alternate, keys.command, keys.numeric_pad, keys.help, keys.function);
					break;
				}
				//case NSKeyDown:
				case 10:
				{
                    uint pos = 0;
					id inputText = ((id (*)(id, SEL))objc_msgSend)(event, charactersSel);
					const char * inputTextUTF8 = ((const char* (*)(id, SEL))objc_msgSend)(inputText, UTF8StringSel);
					uint16_t keyCode = ((unsigned short (*)(id, SEL))objc_msgSend)(event, keyCodeSel);
					if((last_modifyer_keys & 16) && (last_modifyer_keys & 2) && keyCode == BETRAY_BUTTON_Z)
					{
						betray_plugin_button_set(0, BETRAY_BUTTON_REDO, TRUE, -1);
					}else if((last_modifyer_keys & 16) && keyCode == BETRAY_BUTTON_Z)
					{
						betray_plugin_button_set(0, BETRAY_BUTTON_UNDO, TRUE, -1);
					}else if((last_modifyer_keys & 16) && keyCode == BETRAY_BUTTON_X)
					{
						betray_plugin_button_set(0, BETRAY_BUTTON_CUT, TRUE, -1);
					}else if((last_modifyer_keys & 16) && keyCode == BETRAY_BUTTON_C)
					{
						betray_plugin_button_set(0, BETRAY_BUTTON_COPY, TRUE, -1);
					}else if((last_modifyer_keys & 16) && keyCode == BETRAY_BUTTON_V)
					{
						betray_plugin_button_set(0, BETRAY_BUTTON_PASTE, TRUE, -1);
					}else
						betray_plugin_button_set(0, keyCode, TRUE, f_utf8_to_uint32(inputTextUTF8, &pos));
					printf("key down %u, text '%s'\n", keyCode, inputTextUTF8);
				}
                break;
				//case NSKeyUp:
				case 11:
				{
                    uint pos = 0;
                    id inputText = ((id (*)(id, SEL))objc_msgSend)(event, charactersSel);
                    const char * inputTextUTF8 = ((const char* (*)(id, SEL))objc_msgSend)(inputText, UTF8StringSel);
                    uint16_t keyCode = ((unsigned short (*)(id, SEL))objc_msgSend)(event, keyCodeSel);
                    betray_plugin_button_set(0, keyCode, FALSE, f_utf8_to_uint32(inputTextUTF8, &pos));
                    printf("key up %u\n", keyCode);
					break;
				}
				default:
					break;
			}

			//[NSApp sendEvent:event];
			((void (*)(id, SEL, id))objc_msgSend)(NSApp, sendEventSel, event);

			// if user closes the window we might need to terminate asap
			if(terminated)
				break;


		}

		// do runloop stuff
		//[openGLContext update]; // probably we only need to do it when we resize the window
	//	((void (*)(id, SEL))objc_msgSend)(betray_opengl_context_current, updateSel);

		//[openGLContext makeCurrentContext];
	//	((void (*)(id, SEL))objc_msgSend)(betray_opengl_context_current, makeCurrentContextSel);

		//NSRect rect = [contentView frame];
		NSRect rect = ((NSRect (*)(id, SEL))abi_objc_msgSend_stret)(betray_content_view, frameSel);

		//rect = [contentView convertRectToBacking:rect];
		rect = ((NSRect (*)(id, SEL, NSRect))abi_objc_msgSend_stret)(betray_content_view, convertRectToBackingSel, rect);
        betray_reshape_view(rect.size.width, rect.size.height);
        for(i = 0; i < input->pointer_count; i++)
        {
            input->pointers[i].delta_pointer_x += input->pointers[i].pointer_x;
            input->pointers[i].delta_pointer_y += input->pointers[i].pointer_y;
            for(j = 0; j < input->pointers[i].button_count && j < B_POINTER_BUTTONS_COUNT; j++)
            {
                if(input->pointers[i].button[j] && !input->pointers[i].last_button[j])
                {
                    input->pointers[i].click_pointer_x[j] = input->pointers[i].pointer_x;
                    input->pointers[i].click_pointer_y[j] = input->pointers[i].pointer_y;
                }
            }
        }
        betray_plugin_callback_main(input);
      //  if(event_timer < 3)
        {
            betray_action(BAM_EVENT);
            event_timer++;
        }
        betray_action(BAM_DRAW);
#ifdef BETRAY_CONTEXT_OPENGL
        glFinish();
        //[NSApp updateWindows];
        ((void (*)(id, SEL))objc_msgSend)(betray_opengl_context_current, flushBufferSel);
        ((void (*)(id, SEL))objc_msgSend)(betray_opengl_context_current, updateSel);
        ((void (*)(id, SEL))objc_msgSend)(NSApp, updateWindowsSel);
#endif
#ifdef BETRAY_CONTEXT_OPENGLES
        eglSwapBuffers(sEGLDisplay, sEGLSurface);
#endif
        input->frame_number++;
        betray_time_update();
        betray_action(BAM_MAIN);
        input->button_event_count = 0;
        betray_plugin_pointer_clean();
        for(i = 0; i < input->pointer_count; i++)
        {
            input->pointers[i].delta_pointer_x = -input->pointers[i].pointer_x;
            input->pointers[i].delta_pointer_y = -input->pointers[i].pointer_y;
            for(j = 0; j < input->pointers[i].button_count; j++)
                input->pointers[i].last_button[j] = input->pointers[i].button[j];
        }
    }

	printf("gracefully terminated\n");

	#ifdef ARC_AVAILABLE
	}
	#else
	//[pool drain];
//	((void (*)(id, SEL))objc_msgSend)(powl, sel_registerName("drain"));
	#endif
}

uint betray_support_functionality(BSupportedFunctionality funtionality)
{
    uint array[] = {
        256, /*B_SF_USER_COUNT_MAX*/
        256, /*B_SF_POINTER_COUNT_MAX*/
        3, /*B_SF_POINTER_BUTTON_COUNT*/
        TRUE, /*B_SF_FULLSCREEN*/
        TRUE, /*B_SF_WINDOWED*/
        FALSE, /*B_SF_VIEW_POSITION*/
        FALSE, /*B_SF_VIEW_ROTATION*/
        FALSE, /*B_SF_MOUSE_WARP*/
        FALSE, /*B_SF_EXECUTE*/
        FALSE, /*B_SF_REQUESTER*/
        FALSE}; /*B_SF_CLIPBOARD*/
    if(funtionality >= B_SF_COUNT)
        return FALSE;
    return array[funtionality];
}


char *betray_requester_save_get(void *id)
{
    return NULL;
}

void betray_requester_save(char **types, uint type_count, void *id)
{
}

char *betray_requester_load_get(void *id)
{
    return NULL;
}

void betray_requester_load(char **types, uint type_count, void *id)
{
}

char *betray_clipboard_get()
{
     return NULL;
}

void betray_clipboard_set(char *text)
{
}

void betray_button_keyboard(uint user_id, boolean show)
{
}

void betray_desktop_size_get(uint *size_x, uint *size_y)
{
	if(betray_screen_screen_resolution_x == 0)
	{
		Class NSScreenClass = objc_getClass("NSScreen");
		SEL mainScreensSel = sel_registerName("mainScreen");
		id mainScreen = ((id (*)(Class, SEL))objc_msgSend)(NSScreenClass, mainScreensSel);
		SEL screensSel = sel_registerName("screens");
		id screen_array = ((id (*)(Class, SEL))objc_msgSend)(NSScreenClass, screensSel);
		SEL countSel = sel_registerName("count");
		uint array_length = ((uint (*)(Class, SEL))objc_msgSend)(screen_array, countSel);

		SEL objectAtIndexSel = sel_registerName("objectAtIndex:");
		id specific_screen = ((id (*)(Class, SEL, uint))objc_msgSend)(screen_array, objectAtIndexSel, 0);
		SEL visibleFrameSel = sel_registerName("frame");
		NSRect rect = ((NSRect (*)(id, SEL))abi_objc_msgSend_stret)(specific_screen, visibleFrameSel);
        
		betray_screen_screen_resolution_x = rect.size.width;
		betray_screen_screen_resolution_y = rect.size.height;
	}
    if(size_x != NULL)
        *size_x = betray_screen_screen_resolution_x;
    if(size_y != NULL)
        *size_y = betray_screen_screen_resolution_y;
}

boolean b_win32_screen_mode(uint size_x, uint size_y)
{
    return FALSE;
}

boolean betray_activate_context(void *context)
{
    SEL makeCurrentContextSel;
    if(context == NULL)
        context = betray_opengl_context_default;
    if(context == betray_opengl_context_current)
        return TRUE;
    makeCurrentContextSel = sel_registerName("makeCurrentContext");
    ((void (*)(id, SEL))objc_msgSend)(context, makeCurrentContextSel);
    return TRUE;
}

void *b_create_context()
{
    return NULL;
}




void system_wrapper_lose_focus(void)
{
 //   input_focus = FALSE;
}

void betray_set_mouse_warp(boolean warp)
{
 //   mouse_warp = warp;
}


void betray_set_mouse_move(float x, float y)
{
 /*   BInputState *input;
    input = betray_get_input_state();
    mouse_warp_move_x = x - input->pointers[0].pointer_x;
    mouse_warp_move_y = input->pointers[0].pointer_y - y;
    mouse_warp_move = TRUE;*/
}

uint betray_support_context(BContextType context_type)
{
    return context_type == B_CT_OPENGL || context_type == B_CT_OPENGL_OR_ES;
}

extern uint BGlobal_draw_state_fbo;

void *betray_get_proc_address(const char *name)
{
    static const char * gl_lib_path = "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL";
    static void *lib = NULL;
    if(lib == NULL)
        lib = dlopen(gl_lib_path, RTLD_LAZY);
    return dlsym(lib, name);
 }

void betray_glBindFramebufferEXT(GLenum target, GLuint framebuffer)
{
    static void (*internal_glBindFramebufferEXT)(GLenum target, GLuint framebuffer) = NULL;
    if(internal_glBindFramebufferEXT == NULL)
        internal_glBindFramebufferEXT = (void (*)(GLenum , GLuint))betray_get_proc_address("glBindFramebufferEXT");
    if(framebuffer == 0)
        internal_glBindFramebufferEXT(target, BGlobal_draw_state_fbo);
    else
        internal_glBindFramebufferEXT(target, framebuffer);
}

void *betray_gl_proc_address_get_internal(const char *text)
{
#ifdef BETRAY_CONTEXT_OPENGL
 // if(b_win32_opengl_context_current == b_win32_opengl_context)
    {
        uint i;
        char *extension = "glBindFramebuffer";
        for(i = 0; extension[i] == text[i] && extension[i] != 0; i++);
        if(extension[i] == 0)
            return betray_glBindFramebufferEXT;
    }
    return betray_get_proc_address(text);
#endif
    return NULL;
}

void *betray_gl_proc_address_get()
{
    return (void *(*)(const char *))betray_gl_proc_address_get_internal;
}

#endif
#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>
#import <QuartzCore/CAMetalLayer.h>

namespace cct::gfx::vk
{
	id GetMetalLayerFromView(void* window)
	{
		id any = (__bridge id)window;

		NSView* view = nil;
		if ([any isKindOfClass:[NSView class]])
			view = (NSView*)any;
		else if ([any isKindOfClass:[NSWindow class]])
			view = [(NSWindow*)any contentView];
		else
			return nil;

		[view setWantsLayer:YES];
		if (![view.layer isKindOfClass:[CAMetalLayer class]])
		{
            CAMetalLayer* metalLayer = [CAMetalLayer layer];
            view.layer = metalLayer;
		}
		return (CAMetalLayer*)view.layer;
	}
}

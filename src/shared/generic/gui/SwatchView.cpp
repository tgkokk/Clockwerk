/*
 * Copyright 2001-2009, Stephan Aßmus <superstippi@gmx.de>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <stdio.h>

#include <Bitmap.h>
#include <Cursor.h>
#include <LayoutUtils.h>
#include <Looper.h>
#include <Message.h>
#include <TypeConstants.h>
#include <Window.h>

#include "cursors.h"
#include "support.h"
#include "support_ui.h"

#include "SwatchView.h"

#define DRAG_INIT_DIST 10.0

// constructor
SwatchView::SwatchView(const char* name, BMessage* message, BHandler* target,
		rgb_color color, float width, float height)
	:
	BView(name, B_WILL_DRAW),
	fColor(color),
	fTrackingStart(-1.0, -1.0),
	fActive(false),
	fDropInvokes(false),
	fClickMessage(message),
	fDroppedMessage(NULL),
	fTarget(target),
	fWidth(width),
	fHeight(height)
{
	SetViewColor(B_TRANSPARENT_32_BIT);
	SetHighColor(fColor);
}

// destructor
SwatchView::~SwatchView()
{
	delete fClickMessage;
	delete fDroppedMessage;
}

// GetPreferredSize
void
SwatchView::GetPreferredSize(float* width, float* height)
{
	if (width != NULL)
		*width = fWidth;
	if (height != NULL)
		*height = fHeight;
}

// MinSize
BSize
SwatchView::MinSize()
{
	BSize size;
	GetPreferredSize(&size.width, &size.height);
	return BLayoutUtils::ComposeSize(ExplicitMinSize(), size);
}

// MaxSize
BSize
SwatchView::MaxSize()
{
	BSize size;
	GetPreferredSize(&size.width, &size.height);
	if (size.width < 6.0f)
		size.width = B_SIZE_UNLIMITED;
	if (size.height < 6.0f)
		size.height = B_SIZE_UNLIMITED;
	return BLayoutUtils::ComposeSize(ExplicitMaxSize(), size);
}

// PreferredSize
BSize
SwatchView::PreferredSize()
{
	BSize size;
	GetPreferredSize(&size.width, &size.height);
	return BLayoutUtils::ComposeSize(ExplicitPreferredSize(), size);
}

// Draw
void
SwatchView::Draw(BRect updateRect)
{
//	rgb_color background = ui_color(B_PANEL_BACKGROUND_COLOR);
//	rgb_color shadow = tint_color(background, B_DARKEN_2_TINT);
//	rgb_color light = tint_color(background, B_LIGHTEN_MAX_TINT);
//	rgb_color darkShadow = tint_color(background, B_DARKEN_3_TINT);
//	rgb_color lightShadow = tint_color(background, B_DARKEN_1_TINT);
	rgb_color colorLight = tint_color(fColor, B_LIGHTEN_2_TINT);
	rgb_color colorShadow = tint_color(fColor, B_DARKEN_2_TINT);
	BRect r(Bounds());
//	_StrokeRect(r, background, background);
//	r.InsetBy(1.0, 1.0);
/*	_StrokeRect(r, lightShadow, light);
	r.InsetBy(1.0, 1.0);
	_StrokeRect(r, darkShadow, shadow);
	r.InsetBy(1.0, 1.0);*/
	_StrokeRect(r, colorLight, colorShadow);
	r.InsetBy(1.0, 1.0);
	FillRect(r);
}

// MessageReceived
void
SwatchView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_PASTE: {
			rgb_color color;
			if (restore_color_from_message(message,
										   color) >= B_OK) {
				SetColor(color);
				_Invoke(fDroppedMessage);
			}
			break;
		}
		default:
			BView::MessageReceived(message);
			break;
	}
}

// MouseDown
void
SwatchView::MouseDown(BPoint where)
{
	if (Bounds().Contains(where))
		fTrackingStart = where;
}

// MouseUp
void
SwatchView::MouseUp(BPoint where)
{
	if (Bounds().Contains(where)
		&& Bounds().Contains(fTrackingStart))
		_Invoke(fClickMessage);
	fTrackingStart.x = -1.0;
	fTrackingStart.y = -1.0;
}

// MouseMoved
void
SwatchView::MouseMoved(BPoint where, uint32 transit,
					   const BMessage* dragMessage)
{
	if (transit == B_ENTERED_VIEW) {
		BCursor cursor(kDropperCursor);
		SetViewCursor(&cursor, true);
	}
	if (Bounds().Contains(fTrackingStart)) {
		if (point_point_distance(where, fTrackingStart)
			> DRAG_INIT_DIST || transit == B_EXITED_VIEW) {
			_DragColor();
			fTrackingStart.x = -1.0;
			fTrackingStart.y = -1.0;
		}
	}
}

// SetColor
void
SwatchView::SetColor(rgb_color color)
{
	fColor = color;
	SetHighColor(fColor);
	Invalidate();
}

// SetClickedMessage
void
SwatchView::SetClickedMessage(BMessage* message)
{
	delete fClickMessage;
	fClickMessage = message;
}

// SetDroppedMessage
void
SwatchView::SetDroppedMessage(BMessage* message)
{
	delete fDroppedMessage;
	fDroppedMessage = message;
}

// _Invoke
void
SwatchView::_Invoke(const BMessage* _message)
{
	if (_message) {
		BHandler* target = fTarget ? fTarget
							: dynamic_cast<BHandler*>(Window());
		BLooper* looper;
		if (target && (looper = target->Looper())) {
			BMessage message(*_message);
			message.AddPointer("be:source", (void*)this);
			message.AddInt64("be:when", system_time());
			message.AddBool("begin", true);
			store_color_in_message(&message, fColor);
			looper->PostMessage(&message, target);
		}
	}
}

// _StrokeRect
void
SwatchView::_StrokeRect(BRect r, rgb_color leftTop,
					   rgb_color rightBottom)
{
	BeginLineArray(4);
		AddLine(BPoint(r.left, r.bottom),
				BPoint(r.left, r.top), leftTop);
		AddLine(BPoint(r.left + 1.0, r.top),
				BPoint(r.right, r.top), leftTop);
		AddLine(BPoint(r.right, r.top + 1.0),
				BPoint(r.right, r.bottom), rightBottom);
		AddLine(BPoint(r.right - 1.0, r.bottom),
				BPoint(r.left + 1.0, r.bottom), rightBottom);
	EndLineArray();
}

// _DragColor
void
SwatchView::_DragColor()
{
	BBitmap *bitmap = new BBitmap(BRect(0.0, 0.0, 15.0, 15.0), B_RGB32);
	BMessage message = make_color_drop_message(fColor, bitmap);

	DragMessage(&message, bitmap, B_OP_ALPHA, BPoint(9.0, 9.0));
}


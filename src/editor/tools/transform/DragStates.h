/*
 * Copyright 2002-2009, Stephan Aßmus <superstippi@gmx.de>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#ifndef DRAG_STATES_H
#define DRAG_STATES_H

#include <Point.h>

#include "AffineTransform.h"

class BView;
class TransformBox;

// TODO: remove and replace with translation string indices once multiple
// languages are supported
#define TRANSFORMATION 0
#define ROTATE 0
#define MOVE 0
#define SCALE 0
#define MOVE_PIVOT 0
// 

// base class
class DragState {
 public:
								DragState(TransformBox* parent);
	virtual						~DragState() {}

	virtual	void				SetOrigin(BPoint origin);
	virtual	void				DragTo(BPoint current, uint32 modifiers) = 0;
	virtual	void				UpdateViewCursor(BView* view, BPoint current) const = 0;

	virtual	const char*			ActionName() const;
	virtual	uint32				ActionNameIndex() const;

 protected:
			void				_SetViewCursor(BView* view,
											   const uchar* cursorData) const;

			BPoint				fOrigin;
			TransformBox*		fParent;
};

class DragCornerState : public DragState {
 public:
	enum {
		LEFT_TOP_CORNER,
		RIGHT_TOP_CORNER,
		LEFT_BOTTOM_CORNER,
		RIGHT_BOTTOM_CORNER,
	};
								DragCornerState(TransformBox* parent,
												  uint32 corner);
	virtual						~DragCornerState() {}

	virtual	void				SetOrigin(BPoint origin);
	virtual	void				DragTo(BPoint current, uint32 modifiers);
	virtual	void				UpdateViewCursor(BView* view, BPoint current) const;

	virtual	const char*			ActionName() const;
	virtual	uint32				ActionNameIndex() const;

 private:
			uint32				fCorner;

			BPoint				fOffsetFromCorner;
			AffineTransform		fOldTransform;
};

class DragSideState : public DragState {
 public:
	enum {
		LEFT_SIDE,
		TOP_SIDE,
		RIGHT_SIDE,
		BOTTOM_SIDE,
	};
								DragSideState(TransformBox* parent,
											  uint32 side);
	virtual						~DragSideState() {}

	virtual	void				SetOrigin(BPoint origin);
	virtual	void				DragTo(BPoint current, uint32 modifiers);
	virtual	void				UpdateViewCursor(BView* view, BPoint current) const;

	virtual	const char*			ActionName() const;
	virtual	uint32				ActionNameIndex() const;

 private:
			uint32				fSide;

			float				fOffsetFromSide;
			AffineTransform		fOldTransform;
};

// translate state
class DragBoxState : public DragState {
 public:
								DragBoxState(TransformBox* parent)
									: DragState(parent) {}
	virtual						~DragBoxState() {}

	virtual	void				SetOrigin(BPoint origin);
	virtual	void				DragTo(BPoint current, uint32 modifiers);
	virtual	void				UpdateViewCursor(BView* view, BPoint current) const;

	virtual	const char*			ActionName() const;
	virtual	uint32				ActionNameIndex() const;

 private:
			BPoint				fOldTranslation;
};

// rotate state
class RotateBoxState : public DragState {
 public:
								RotateBoxState(TransformBox* parent);
	virtual						~RotateBoxState() {}

	virtual	void				SetOrigin(BPoint origin);
	virtual	void				DragTo(BPoint current, uint32 modifiers);
	virtual	void				UpdateViewCursor(BView* view, BPoint current) const;

	virtual	const char*			ActionName() const;
	virtual	uint32				ActionNameIndex() const;

 private:
			double				fOldAngle;
};

// offset center state
class OffsetCenterState : public DragState {
 public:
								OffsetCenterState(TransformBox* parent)
									: DragState(parent) {}
	virtual						~OffsetCenterState() {}

	virtual	void				SetOrigin(BPoint origin);
	virtual	void				DragTo(BPoint current, uint32 modifiers);
	virtual	void				UpdateViewCursor(BView* view, BPoint current) const;

	virtual	const char*			ActionName() const;
	virtual	uint32				ActionNameIndex() const;

};

#endif // DRAG_STATES_H

#pragma once
class ShapeComponent {
public:
    virtual void draw() = 0;
    virtual ~ShapeComponent() = default; // virtual destructor
};

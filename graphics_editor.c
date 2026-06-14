#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_WIDTH 100
#define MAX_HEIGHT 50
#define MAX_SHAPES 100
typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_TRIANGLE,
    SHAPE_CIRCLE
} ShapeType;
typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;
typedef struct {
    int x, y;
    int width, height;
} RectParams;
typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriParams;
typedef struct {
    int xc, yc;
    int r;
} CircleParams;
typedef struct {
    int id;
    ShapeType type;
    union {
        LineParams line;
        RectParams rect;
        TriParams tri;
        CircleParams circle;
    } data;
} Shape;
typedef struct {
    int width;
    int height;
    char grid[MAX_HEIGHT][MAX_WIDTH];
} Canvas;
// Function declarations
void init_canvas(Canvas* canvas, int width, int height);
void clear_canvas(Canvas* canvas);
void draw_pixel(Canvas* canvas, int x, int y, char c);
void display_canvas(const Canvas* canvas);
void draw_line(Canvas* canvas, int x1, int y1, int x2, int y2, char c);
void draw_rectangle(Canvas* canvas, int x, int y, int width, int height, char c);
void draw_triangle(Canvas* canvas, int x1, int y1, int x2, int y2, int x3, int y3, char c);
void draw_circle(Canvas* canvas, int xc, int yc, int r, char c);
void render_shapes(Canvas* canvas, const Shape* shapes, int num_shapes);
const char* shape_type_to_string(ShapeType type);
int get_int_with_default(const char* prompt, int default_val, int min_val, int max_val);
int find_shape_index(const Shape* shapes, int num_shapes, int id);
void print_shape_details(const Shape* s);
void prompt_shape_data(const Canvas* canvas, ShapeType type, Shape* s);
// Global state
Shape shapes[MAX_SHAPES];
int num_shapes = 0;
int next_id = 1;
Canvas canvas;
// Canvas operations
void init_canvas(Canvas* canvas, int width, int height) {
    if (width < 1) width = 1;
    if (width > MAX_WIDTH) width = MAX_WIDTH;
    if (height < 1) height = 1;
    if (height > MAX_HEIGHT) height = MAX_HEIGHT;
    
    canvas->width = width;
    canvas->height = height;
    clear_canvas(canvas);
}
void clear_canvas(Canvas* canvas) {
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            canvas->grid[y][x] = '_';
        }
    }
}
void draw_pixel(Canvas* canvas, int x, int y, char c) {
    if (x >= 0 && x < canvas->width && y >= 0 && y < canvas->height) {
        canvas->grid[y][x] = c;
    }
}
void display_canvas(const Canvas* canvas) {
    // Top border
    printf("   +");
    for (int x = 0; x < canvas->width; x++) {
        printf("-");
    }
    printf("+\n");
    
    // Canvas grid with row numbering
    for (int y = 0; y < canvas->height; y++) {
        printf("%2d |", y);
        for (int x = 0; x < canvas->width; x++) {
            putchar(canvas->grid[y][x]);
        }
        printf("|\n");
    }
    
    // Bottom border
    printf("   +");
    for (int x = 0; x < canvas->width; x++) {
        printf("-");
    }
    printf("+\n");
    
    // Column numbering helper (units digit for alignment)
    printf("    ");
    for (int x = 0; x < canvas->width; x++) {
        printf("%d", x % 10);
    }
    printf("\n");
}
// Drawing algorithms
void draw_line(Canvas* canvas, int x1, int y1, int x2, int y2, char c) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        draw_pixel(canvas, x1, y1, c);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}
void draw_rectangle(Canvas* canvas, int x, int y, int width, int height, char c) {
    if (width <= 0 || height <= 0) return;
    int x2 = x + width - 1;
    int y2 = y + height - 1;
    
    draw_line(canvas, x, y, x2, y, c);
    draw_line(canvas, x, y2, x2, y2, c);
    draw_line(canvas, x, y, x, y2, c);
    draw_line(canvas, x2, y, x2, y2, c);
}
void draw_triangle(Canvas* canvas, int x1, int y1, int x2, int y2, int x3, int y3, char c) {
    draw_line(canvas, x1, y1, x2, y2, c);
    draw_line(canvas, x2, y2, x3, y3, c);
    draw_line(canvas, x3, y3, x1, y1, c);
}
void draw_circle(Canvas* canvas, int xc, int yc, int r, char c) {
    if (r < 0) return;
    if (r == 0) {
        draw_pixel(canvas, xc, yc, c);
        return;
    }
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    
    while (y >= x) {
        draw_pixel(canvas, xc + x, yc + y, c);
        draw_pixel(canvas, xc - x, yc + y, c);
        draw_pixel(canvas, xc + x, yc - y, c);
        draw_pixel(canvas, xc - x, yc - y, c);
        draw_pixel(canvas, xc + y, yc + x, c);
        draw_pixel(canvas, xc - y, yc + x, c);
        draw_pixel(canvas, xc + y, yc - x, c);
        draw_pixel(canvas, xc - y, yc - x, c);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
void render_shapes(Canvas* canvas, const Shape* shapes, int num_shapes) {
    clear_canvas(canvas);
    for (int i = 0; i < num_shapes; i++) {
        const Shape* s = &shapes[i];
        switch (s->type) {
            case SHAPE_LINE:
                draw_line(canvas, s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2, '*');
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(canvas, s->data.rect.x, s->data.rect.y, s->data.rect.width, s->data.rect.height, '*');
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(canvas, s->data.tri.x1, s->data.tri.y1, s->data.tri.x2, s->data.tri.y2, s->data.tri.x3, s->data.tri.y3, '*');
                break;
            case SHAPE_CIRCLE:
                draw_circle(canvas, s->data.circle.xc, s->data.circle.yc, s->data.circle.r, '*');
                break;
        }
    }
}
const char* shape_type_to_string(ShapeType type) {
    switch (type) {
        case SHAPE_LINE: return "Line";
        case SHAPE_RECTANGLE: return "Rectangle";
        case SHAPE_TRIANGLE: return "Triangle";
        case SHAPE_CIRCLE: return "Circle";
        default: return "Unknown";
    }
}
// CLI helpers
int get_int_with_default(const char* prompt, int default_val, int min_val, int max_val) {
    char buffer[128];
    while (1) {
        printf("%s [%d]: ", prompt, default_val);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return default_val;
        }
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
            buffer[len-1] = '\0';
            len--;
        }
        if (len == 0) {
            return default_val;
        }
        char* endptr;
        long val = strtol(buffer, &endptr, 10);
        if (endptr == buffer || *endptr != '\0') {
            printf("Error: Invalid integer format. Please try again.\n");
            continue;
        }
        if (val < min_val || val > max_val) {
            printf("Error: Value out of range (%d to %d). Please try again.\n", min_val, max_val);
            continue;
        }
        return (int)val;
    }
}
int find_shape_index(const Shape* shapes, int num_shapes, int id) {
    for (int i = 0; i < num_shapes; i++) {
        if (shapes[i].id == id) {
            return i;
        }
    }
    return -1;
}
void print_shape_details(const Shape* s) {
    printf("[ID: %d] %s: ", s->id, shape_type_to_string(s->type));
    switch (s->type) {
        case SHAPE_LINE:
            printf("Start (%d, %d) -> End (%d, %d)\n", 
                   s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2);
            break;
        case SHAPE_RECTANGLE:
            printf("Top-left (%d, %d), Width %d, Height %d\n", 
                   s->data.rect.x, s->data.rect.y, s->data.rect.width, s->data.rect.height);
            break;
        case SHAPE_TRIANGLE:
            printf("Vertices: (%d, %d), (%d, %d), (%d, %d)\n", 
                   s->data.tri.x1, s->data.tri.y1, 
                   s->data.tri.x2, s->data.tri.y2, 
                   s->data.tri.x3, s->data.tri.y3);
            break;
        case SHAPE_CIRCLE:
            printf("Center (%d, %d), Radius %d\n", 
                   s->data.circle.xc, s->data.circle.yc, s->data.circle.r);
            break;
    }
}
void prompt_shape_data(const Canvas* canvas, ShapeType type, Shape* s) {
    s->type = type;
    int max_x = canvas->width - 1;
    int max_y = canvas->height - 1;
    
    switch (type) {
        case SHAPE_LINE:
            s->data.line.x1 = get_int_with_default("Enter Start X1", 0, 0, max_x);
            s->data.line.y1 = get_int_with_default("Enter Start Y1", 0, 0, max_y);
            s->data.line.x2 = get_int_with_default("Enter End X2", max_x, 0, max_x);
            s->data.line.y2 = get_int_with_default("Enter End Y2", max_y, 0, max_y);
            break;
            
        case SHAPE_RECTANGLE:
            s->data.rect.x = get_int_with_default("Enter Top-Left X", 0, 0, max_x);
            s->data.rect.y = get_int_with_default("Enter Top-Left Y", 0, 0, max_y);
            s->data.rect.width = get_int_with_default("Enter Width", 5, 1, canvas->width);
            s->data.rect.height = get_int_with_default("Enter Height", 5, 1, canvas->height);
            break;
            
        case SHAPE_TRIANGLE:
            s->data.tri.x1 = get_int_with_default("Enter Vertex 1 X1", 0, 0, max_x);
            s->data.tri.y1 = get_int_with_default("Enter Vertex 1 Y1", 0, 0, max_y);
            s->data.tri.x2 = get_int_with_default("Enter Vertex 2 X2", max_x / 2, 0, max_x);
            s->data.tri.y2 = get_int_with_default("Enter Vertex 2 Y2", max_y, 0, max_y);
            s->data.tri.x3 = get_int_with_default("Enter Vertex 3 X3", max_x, 0, max_x);
            s->data.tri.y3 = get_int_with_default("Enter Vertex 3 Y3", 0, 0, max_y);
            break;
            
        case SHAPE_CIRCLE:
            s->data.circle.xc = get_int_with_default("Enter Center X", max_x / 2, 0, max_x);
            s->data.circle.yc = get_int_with_default("Enter Center Y", max_y / 2, 0, max_y);
            s->data.circle.r = get_int_with_default("Enter Radius", 3, 0, MAX_WIDTH);
            break;
    }
}
int main() {
    printf("=== 2D Graphics Editor (C Version) ===\n\n");
    
    int w = get_int_with_default("Enter canvas width (1-100)", 40, 1, MAX_WIDTH);
    int h = get_int_with_default("Enter canvas height (1-50)", 20, 1, MAX_HEIGHT);
    
    init_canvas(&canvas, w, h);
    
    while (1) {
        render_shapes(&canvas, shapes, num_shapes);
        printf("\n");
        display_canvas(&canvas);
        printf("\n");
        
        printf("--- Active Objects ---\n");
        if (num_shapes == 0) {
            printf("(No objects added yet)\n");
        } else {
            for (int i = 0; i < num_shapes; i++) {
                print_shape_details(&shapes[i]);
            }
        }
        printf("----------------------\n\n");
        
        printf("Options:\n");
        printf(" 1. Add a shape\n");
        printf(" 2. Delete a shape\n");
        printf(" 3. Modify a shape\n");
        printf(" 4. Resize canvas\n");
        printf(" 5. Clear all shapes\n");
        printf(" 6. Exit\n");
        
        int choice = get_int_with_default("Enter choice", 1, 1, 6);
        
        if (choice == 1) {
            if (num_shapes >= MAX_SHAPES) {
                printf("Error: Maximum shape limit (%d) reached.\n", MAX_SHAPES);
                continue;
            }
            printf("\nSelect Shape Type:\n");
            printf(" 1. Line\n");
            printf(" 2. Rectangle\n");
            printf(" 3. Triangle\n");
            printf(" 4. Circle\n");
            int type_choice = get_int_with_default("Enter shape type", 1, 1, 4);
            
            Shape new_shape;
            new_shape.id = next_id++;
            prompt_shape_data(&canvas, (ShapeType)type_choice, &new_shape);
            
            shapes[num_shapes++] = new_shape;
            printf("Shape added successfully with ID: %d!\n", new_shape.id);
            
        } else if (choice == 2) {
            if (num_shapes == 0) {
                printf("No shapes to delete.\n");
                continue;
            }
            int del_id = get_int_with_default("Enter shape ID to delete", shapes[0].id, 1, next_id - 1);
            int idx = find_shape_index(shapes, num_shapes, del_id);
            if (idx == -1) {
                printf("Error: Shape with ID %d not found.\n", del_id);
                continue;
            }
            for (int i = idx; i < num_shapes - 1; i++) {
                shapes[i] = shapes[i + 1];
            }
            num_shapes--;
            printf("Shape with ID %d deleted successfully.\n", del_id);
            
        } else if (choice == 3) {
            if (num_shapes == 0) {
                printf("No shapes to modify.\n");
                continue;
            }
            int mod_id = get_int_with_default("Enter shape ID to modify", shapes[0].id, 1, next_id - 1);
            int idx = find_shape_index(shapes, num_shapes, mod_id);
            if (idx == -1) {
                printf("Error: Shape with ID %d not found.\n", mod_id);
                continue;
            }
            printf("\nModifying shape details:\n");
            print_shape_details(&shapes[idx]);
            
            prompt_shape_data(&canvas, shapes[idx].type, &shapes[idx]);
            printf("Shape ID %d modified successfully.\n", mod_id);
            
        } else if (choice == 4) {
            int new_w = get_int_with_default("Enter new canvas width", canvas.width, 1, MAX_WIDTH);
            int new_h = get_int_with_default("Enter new canvas height", canvas.height, 1, MAX_HEIGHT);
            init_canvas(&canvas, new_w, new_h);
            printf("Canvas resized successfully.\n");
            
        } else if (choice == 5) {
            num_shapes = 0;
            printf("All shapes cleared.\n");
            
        } else if (choice == 6) {
            printf("Exiting editor. Goodbye!\n");
            break;
        }
    }
    
    return 0;
}

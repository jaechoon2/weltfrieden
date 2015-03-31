#define MAXSHADERLAYERS 20


enum shaderstate {UNUSED, UNINITIALIZED, INITIALIZED};
typedef struct 
{
  enum shaderstate state;
  float gain; // maps to opacity of shader layer
  char *filename;
  char *filecontent;
  float duration;
  float end;

  unsigned int progId;
  unsigned int shaderId;
} shader;



void addShaderLayer(shader s);
void useShaderLayer(shader *s);

void removeDeadLayers();

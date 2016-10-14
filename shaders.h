#ifndef SHADERS_H
#define SHADERS_H

#define GLSL(version, shader)  "#version " #version "\n" #shader
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_NORMAL_ATTRIBUTE 1

static const char *vertsrc = GLSL(130,
   in vec3 vertex;
   in vec3 normal;
   in vec2 texCoord;

   uniform mat4 matrixp; // projection
   uniform mat4 matrixv; // view (camera)
   uniform mat4 matrixm; // model (position of object in scene)
   uniform mat3 matrixvn; // normal view (camera)
   uniform mat3 matrixmn; // normal model (position of object in scene)

   out vec3 n;
   out vec3 p;
   out vec2 t;

   void main(void)
   {
       n = matrixvn * matrixmn * normal;

       vec4 vmp = matrixv * matrixm * vec4(vertex, 1.0);
       p = vec3(vmp);

       t = texCoord;
       gl_Position = matrixp * vmp;
   }
);

static const char *fragsrc = GLSL(130,
    in vec3 n; // in view coordinates
    in vec3 p; // in view coordinates
    in vec2 t;

    uniform bool istexture;
    uniform sampler2D texture;

    uniform mat3 matrixvn; // normal view (camera)

    uniform vec4 ambiant;
    uniform vec4 diffuse;
    uniform vec4 specular;
    uniform float hardness;
    uniform vec3 light; // is given in model coordinates

    out vec4 color;

    void main(void)
    {
       vec3 l = matrixvn * light;
       vec3 nn = normalize(n);
       vec3 pp = normalize(p);

       float dfactor = clamp(dot(l, nn), 0.0, 1.0);

       vec3 i = reflect(pp, nn);
       float sfactor = pow(clamp(dot(l, i), 0.0, 1.0), hardness);

       color = ambiant + dfactor * diffuse + sfactor * specular;
       if (istexture)
           color = texture2D(texture, t);
    }
);

#endif // SHADERS_H

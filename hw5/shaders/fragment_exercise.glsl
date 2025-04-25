const float LINEAR_ATTENUATION_FACTOR    = 0.0;
const float QUADRATIC_ATTENUATION_FACTOR = 5.0;

uniform int life_loss;
uniform sampler2D diffuse;
uniform vec2 light_position;

varying vec2 texCoordVar;
varying vec2 pix_position;

float attenuate(float dist, float lin_att, float quad_att)
{
     return 1.0 / (1.0 + (lin_att * dist) + (quad_att * dist  * dist));
}

void main()
{
    if (life_loss == 1) {
        vec4 colour = texture2D(diffuse, texCoordVar);
        gl_FragColor = vec4(colour.r, 0, 0, colour.a);
    } else {
        gl_FragColor = texture2D(diffuse, texCoordVar);
    }
}

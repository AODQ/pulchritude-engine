#version 460 core

in layout(location = 0) vec4 inColor;
in layout(location = 1) vec3 inUv;
in layout(location = 2) vec3 inNormal;
in layout(location = 3) vec3 inOrigin;

out layout(location = 0) vec4 outColor;

// this shader code is from physx in a weekend, need to change to 
// texture later
vec3 GetColorFromPositionAndNormal( in vec3 worldPosition, in vec3 normal ) {
    const float pi = 3.141519;

    vec3 scaledPos = worldPosition.xyz * pi * 2.0;
    vec3 scaledPos2 = worldPosition.xyz * pi * 2.0 / 10.0 + vec3( pi / 4.0 );
    float s = cos( scaledPos2.x ) * cos( scaledPos2.y ) * cos( scaledPos2.z );  // [-1,1] range
    float t = cos( scaledPos.x ) * cos( scaledPos.y ) * cos( scaledPos.z );     // [-1,1] range

    vec3 colorMultiplier = vec3( 0.7, 0.7, 0.9 );
    if ( abs( normal.x ) > abs( normal.y ) && abs( normal.x ) > abs( normal.z ) ) {
        colorMultiplier = vec3( 0.7, 0.9, 0.7 );
    } else if ( abs( normal.y ) > abs( normal.x ) && abs( normal.y ) > abs( normal.z ) ) {
        colorMultiplier = vec3( 0.8, 0.6, 0.7 );
    }

    t = ceil( t * 0.9 );
    s = ( ceil( s * 0.9 ) + 3.0 ) * 0.25;
    vec3 colorB = vec3( 0.85, 0.85, 0.85 );
    vec3 colorA = vec3( 1, 1, 1 );
    vec3 finalColor = mix( colorA, colorB, t ) * s;

    return colorMultiplier * finalColor;
}

vec3 checker()
{
    float dx = 0.25;
    float dy = 0.25;
    vec3 colorMultiplier = vec3( 0.0, 0.0, 0.0 );
    for ( float y = 0.0; y < 1.0; y += dy ) {
        for ( float x = 0.0; x < 1.0; x += dx ) {
            vec3 samplePos = inUv + dFdx( inUv ) * x + dFdy( inUv ) * y;
            vec3 normal = normalize( inUv );
            colorMultiplier += GetColorFromPositionAndNormal( samplePos.xyz, normal.xyz ) * dx * dy;
        }
    }
    return colorMultiplier;
}

void main() {
  outColor = vec4(checker(), 1.0);
  outColor.a = 1.0;
}

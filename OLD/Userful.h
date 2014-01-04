#pragma once

#ifndef _MR_USERFUL_H_
#define _MR_USERFUL_H_

namespace MR{
    const float cube_verts_1x1x1[]{
        //bottom quad
        0, 0, 0, //0
        1, 0, 0, //1
        1, 0, 1, //2
        0, 0, 1, //3

        //upper quad
        0, 1, 0, //4
        1, 1, 0, //5
        1, 1, 1, //6
        0, 1, 1  //7
    };

    const unsigned int cube_verts_1x1x1_num = 24;

    const unsigned int cube_inds_1x1x1_triangles[]{
        //bottom
        0, 1, 2,
        0, 2, 3,
        //upper
        4, 5, 6,
        4, 6, 7,
        //back
        3, 2, 6,
        3, 6, 7,
        //front
        0, 1, 5,
        0, 5, 4,
        //left
        0, 3, 7,
        0, 7, 4,
        //right
        1, 2, 6,
        1, 6, 5
    };

    const unsigned int cube_inds_1x1x1_triangles_num = 36;

    const unsigned int cube_inds_1x1x1_quads[]{
        //bottom
        0, 1, 2, 3,
        //upper
        4, 5, 6, 7,
        //back
        3, 2, 6, 7,
        //front
        0, 1, 5, 4,
        //left
        0, 3, 7, 4,
        //right
        1, 2, 6, 5
    };

    const unsigned int cube_inds_1x1x1_quads_num = 24;

    const char* shader_code_phong_textured_vertex =
    "uniform vec3 fvLightPosition;\
    uniform vec3 fvEyePosition;\
\
    varying vec2 Texcoord;\
    varying vec3 ViewDirection;\
    varying vec3 LightDirection;\
    varying vec3 Normal;\
       \
    void main( void )\
    {\
       gl_Position = ftransform();\
       Texcoord    = gl_MultiTexCoord0.xy;\
        \
       vec4 fvObjectPosition = gl_ModelViewMatrix * gl_Vertex;\
       \
       ViewDirection  = fvEyePosition - fvObjectPosition.xyz;\
       LightDirection = fvLightPosition - fvObjectPosition.xyz;\
       Normal         = gl_NormalMatrix * gl_Normal;\
    }";

    const char* shader_code_phong_textured_fragment =
    "uniform vec4 fvAmbient;\
    uniform vec4 fvSpecular;\
    uniform vec4 fvDiffuse;\
    uniform float fSpecularPower;\
\
    uniform sampler2D baseMap;\
\
    varying vec2 Texcoord;\
    varying vec3 ViewDirection;\
    varying vec3 LightDirection;\
    varying vec3 Normal;\
\
    void main( void )\
    {\
       vec3  fvLightDirection = normalize( LightDirection );\
       vec3  fvNormal         = normalize( Normal );\
       float fNDotL           = dot( fvNormal, fvLightDirection ); \
       \
       vec3  fvReflection     = normalize( ( ( 2.0 * fvNormal ) * fNDotL ) - fvLightDirection ); \
       vec3  fvViewDirection  = normalize( ViewDirection );\
       float fRDotV           = max( 0.0, dot( fvReflection, fvViewDirection ) );\
       \
       vec4  fvBaseColor      = texture2D( baseMap, Texcoord );\
       \
       vec4  fvTotalAmbient   = fvAmbient * fvBaseColor; \
       vec4  fvTotalDiffuse   = fvDiffuse * fNDotL * fvBaseColor; \
       vec4  fvTotalSpecular  = fvSpecular * ( pow( fRDotV, fSpecularPower ) );\
      \
       gl_FragColor = ( fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular );\
    }";

    const char* shader_code_bump_textured_vertex =
    "uniform vec3 fvLightPosition;\
    uniform vec3 fvEyePosition;\
\
    varying vec2 Texcoord;\
    varying vec3 ViewDirection;\
    varying vec3 LightDirection;\
       \
    //attribute vec3 rm_Binormal;\
    //attribute vec3 rm_Tangent;\
       \
    void main( void )\
    {\
        //tangent and binormal from normal \
       vec3 rm_Tangent; \
       vec3 rm_Binormal; \
       \
       vec3 c1 = cross( gl_Normal, vec3(0.0, 0.0, 1.0) );\
       vec3 c2 = cross( gl_Normal, vec3(0.0, 1.0, 0.0) );\
       \
       if( length(c1) > length(c2) ){\
          rm_Tangent = c1;\
       }\
       else{\
          rm_Tangent = c2;\
       }\
       rm_Tangent = normalize(rm_Tangent);\
       rm_Binormal = cross(gl_Normal, rm_Tangent);\
       rm_Binormal = normalize(rm_Binormal);\
       //-------\
       gl_Position = ftransform();\
       Texcoord    = gl_MultiTexCoord0.xy;\
        \
       vec4 fvObjectPosition = gl_ModelViewMatrix * gl_Vertex;\
       \
       vec3 fvViewDirection  = fvEyePosition - fvObjectPosition.xyz;\
       vec3 fvLightDirection = fvLightPosition - fvObjectPosition.xyz;\
         \
       vec3 fvNormal         = /*gl_NormalMatrix * */gl_Normal;\
       vec3 fvBinormal       = /*gl_NormalMatrix * */rm_Binormal;\
       vec3 fvTangent        = /*gl_NormalMatrix * */rm_Tangent;\
          \
       ViewDirection.x  = dot( fvTangent, fvViewDirection );\
       ViewDirection.y  = dot( fvBinormal, fvViewDirection );\
       ViewDirection.z  = dot( fvNormal, fvViewDirection );\
       \
       LightDirection.x  = dot( fvTangent, fvLightDirection.xyz );\
       LightDirection.y  = dot( fvBinormal, fvLightDirection.xyz );\
       LightDirection.z  = dot( fvNormal, fvLightDirection.xyz );\
    }";

    const char* shader_code_bump_textured_fragment =
    "uniform vec4 fvAmbient;\
    uniform vec4 fvSpecular;\
    uniform vec4 fvDiffuse;\
    uniform float fSpecularPower;\
\
    uniform sampler2D baseMap;\
    uniform sampler2D bumpMap;\
\
    varying vec2 Texcoord;\
    varying vec3 ViewDirection;\
    varying vec3 LightDirection;\
\
    void main( void )\
    {\
       vec3  fvLightDirection = normalize( LightDirection );\
       vec3  fvNormal         = normalize( ( texture2D( bumpMap, Texcoord ).xyz * 2.0 ) - 1.0 );\
       float fNDotL           = dot( fvNormal, fvLightDirection ); \
       \
       vec3  fvReflection     = normalize( ( ( 2.0 * fvNormal ) * fNDotL ) - fvLightDirection ); \
       vec3  fvViewDirection  = normalize( ViewDirection );\
       float fRDotV           = max( 0.0, dot( fvReflection, fvViewDirection ) );\
       \
       vec4  fvBaseColor      = texture2D( baseMap, Texcoord );\
       \
       vec4  fvTotalAmbient   = fvAmbient * fvBaseColor; \
       vec4  fvTotalDiffuse   = fvDiffuse * fNDotL * fvBaseColor; \
       vec4  fvTotalSpecular  = fvSpecular * ( pow( fRDotV, fSpecularPower ) );\
      \
       gl_FragColor = ( fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular );\
    }";//( fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular )
}

#endif // _MR_USERFUL_H_

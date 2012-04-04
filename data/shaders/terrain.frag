#version 120

struct TerrainRegion
{
    float min;
    float max;
};

uniform TerrainRegion region1;
uniform TerrainRegion region2;
uniform TerrainRegion region3;
uniform TerrainRegion region4;

uniform sampler2D region1ColorMap;
uniform sampler2D region2ColorMap;
uniform sampler2D region3ColorMap;
uniform sampler2D region4ColorMap;

varying vec4 normal;

vec4 GenerateTerrainColor()
{
    vec4 terrainColor = vec4(0.0, 0.0, 0.0, 1.0);
    float height = normal.w;
    float regionMin = 0.0;
    float regionMax = 0.0;
    float regionRange = 0.0;
    float regionWeight = 0.0;

    // Terrain region 1.
    regionMin = region1.min;
    regionMax = region1.max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
    terrainColor += regionWeight * texture2D(region1ColorMap, gl_TexCoord[0].st);

    // Terrain region 2.
    regionMin = region2.min;
    regionMax = region2.max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
    terrainColor += regionWeight * texture2D(region2ColorMap, gl_TexCoord[0].st);

    // Terrain region 3.
    regionMin = region3.min;
    regionMax = region3.max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
    terrainColor += regionWeight * texture2D(region3ColorMap, gl_TexCoord[0].st);

    // Terrain region 4.
    regionMin = region4.min;
    regionMax = region4.max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
    terrainColor += regionWeight * texture2D(region4ColorMap, gl_TexCoord[0].st);

    return terrainColor;
}

void main()
{
    vec3 n = normalize(normal.xyz);

    float nDotL = max(0.0, dot(n, gl_LightSource[0].position.xyz));

    vec4 ambient = gl_FrontLightProduct[0].ambient;
    vec4 diffuse = gl_FrontLightProduct[0].diffuse * nDotL;
    vec4 color = gl_FrontLightModelProduct.sceneColor + ambient + diffuse;

    gl_FragColor = color * GenerateTerrainColor();
}


uniform vec2 resolution;
uniform vec4 color;

void main()
{
     // Normalized pixel coordinates (from 0 to 1)
	vec2 uv = gl_FragCoord.xy / resolution;

    float y = smoothstep(0.01, 0.99, uv.x) * 0.99 + 0.005;
    float margin = 0.01;
    
    float falloff = smoothstep(0.1, 0.8, abs(uv.y - y)*100.0);
    
    vec4 bg =  vec4(color.rgb, 0.0);
    
    if (uv.y < y + margin && uv.y > y - margin)
    	gl_FragColor = mix(color, bg, falloff);
    else
    	gl_FragColor = bg;
}
#version 410
uniform sampler2D tex; 
out vec4 color; 
uniform int shader_now; 
uniform float iTime;

in VS_OUT
{
vec2 texcoord; 
} fs_in;

float sigma_e = 2.0f; 
float sigma_r = 2.8f; 
float phi = 3.4f; 
float tau = 0.99f; 
float twoSigmaESquared = 2.0 * sigma_e * sigma_e;		
float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;		
int halfWidth = int(ceil(2.0 * sigma_r)); 
vec2 img_size = vec2(800, 600); 
int nbins = 8; 

#define SORT_SIZE  49  
#define MASK_SIZE  7

#define TAU 6.28318530718
#define MAX_ITER 5



const float PHI = 1.61803398874989484820459; // £X = Golden Ratio 

float gold_noise(in vec2 xy, in float seed)
{
    return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}


void main(void)
{

	switch (shader_now)
	{
		case(0):
		{
			color = texture(tex, fs_in.texcoord);
			break;
		}
		case(1):
		{
			color = vec4(0,0,0,0);
			int n = 0;
			int half_size = 3;
			for (int i = -half_size; i <= half_size; ++i) {

				for (int j = -half_size; j <= half_size; ++j) {

					vec4 c = texture(tex, fs_in.texcoord + vec2(i, j) / img_size);
					color += c;
					n++;
				}
			}
			color /= n;
			break;
		}
		case(2): 
		{

			int half_size = int(sqrt(SORT_SIZE)/2);
			vec4 sort_list[SORT_SIZE];
			int n = 0;
			for (int i = -half_size; i <= half_size; ++i) {

				for (int j = -half_size; j <= half_size; ++j) {

					vec4 c = texture(tex, fs_in.texcoord + vec2(i, j) / img_size);
					sort_list[n] = c;
					n++;
				}
			}

			for (int i = 0; i < SORT_SIZE; i++) {
				for (int j = SORT_SIZE - 1; i < j; j--) {
					float tmp_now = sort_list[j].r + sort_list[j].g + sort_list[j].b;
					float tmp_other = sort_list[j - 1].r + sort_list[j - 1].g + sort_list[j - 1].b;
					if (tmp_now < tmp_other) {
						vec4 tmp = sort_list[j];
						sort_list[j] = sort_list[j - 1];
						sort_list[j - 1] = tmp;
					}
				}
			}

			color = sort_list[SORT_SIZE/2];

			break;
		}
		case(3):
		{
			vec2 coord1 = fs_in.texcoord + vec2(-3.0, -3.0) / img_size;
			vec2 coord2 = fs_in.texcoord + vec2(-3.0, 3.0) / img_size;
			vec2 coord3 = fs_in.texcoord + vec2(3.0, -3.0) / img_size;
			vec2 coord4 = fs_in.texcoord + vec2(3.0, 3.0) / img_size;

			vec4 s1 = vec4(texture2D(tex, coord1));
			vec4 s2 = vec4(texture2D(tex, coord2));
			vec4 s3 = vec4(texture2D(tex, coord3));
			vec4 s4 = vec4(texture2D(tex, coord4));

			vec4 tmp1 = mix(s1, s3, fs_in.texcoord.x);
			vec4 tmp2 = mix(s2, s4, fs_in.texcoord.x);

			color = mix(tmp1, tmp2, fs_in.texcoord.y);

			break;
		}
		case(4):
		{
			int half_size = int(MASK_SIZE/2);
			int mask[MASK_SIZE];
			mask[0] = 1;
			for (int i = 1; i<MASK_SIZE; i++) {
				int a = MASK_SIZE - 1;
				int b = 1;
				int tmp_a = a;
				int tmp_b = b;
				for (int j = 1; j<i; j++) {
					a = a * (tmp_a - j);
					b = b * (tmp_b + j);
				}
				mask[i] = a / b;
			}

			for (int i = -half_size; i <= half_size; ++i) {

				for (int j = -half_size; j <= half_size; ++j) {

					vec4 c = texture(tex, fs_in.texcoord + vec2(i, j) / img_size) * mask[i+ half_size] * mask[j+ half_size];
					color += c;
				}
			}
			color /= (pow(2, MASK_SIZE-1)*pow(2, MASK_SIZE - 1));

			break;
		}
		case(5):
		{
			vec4 texture_color = texture(tex, fs_in.texcoord);

			float r = floor(texture_color.r * float(nbins)) / float(nbins);
			float g = floor(texture_color.g * float(nbins)) / float(nbins);
			float b = floor(texture_color.b * float(nbins)) / float(nbins);
			color = vec4(r, g, b, texture_color.a);
			break;
		}
		case(6):
		{
			vec2 sum = vec2(0.0); 
			vec2 norm = vec2(0.0); 
			int kernel_count = 0; 
			for (int i = -halfWidth; i <= halfWidth; ++i) {
	
					for (int j = -halfWidth; j <= halfWidth; ++j) {
			
							float d = length(vec2(i, j)); 
							vec2 kernel = vec2(exp(-d * d / twoSigmaESquared), 
								exp(-d * d / twoSigmaRSquared)); 
							vec4 c = texture(tex, fs_in.texcoord + vec2(i, j) / img_size); 
							vec2 L = vec2(0.299 * c.r + 0.587 * c.g + 0.114 * c.b); 
				
							norm += 2.0 * kernel; 
							sum += kernel * L; 
					}
			}
			sum /= norm; 

			float H = 100.0 * (sum.x - tau * sum.y); 
			float edge = (H > 0.0) ? 1.0 : 2.0 * smoothstep(-2.0, 2.0, phi * H); 

			color = vec4(edge, edge, edge, 1.0); 
			break; 
		}
		case(7):
		{
			color = vec4(0);
			int n = 0;
			int half_size = 3;
			for (int i = -half_size; i <= half_size; ++i) {

				for (int j = -half_size; j <= half_size; ++j) {

					vec4 c = texture(tex, fs_in.texcoord + vec2(i, j) / img_size);
					color += c;
					n++;
				}
			}
			color /= n;

			float r = floor(color.r * float(nbins)) / float(nbins);
			float g = floor(color.g * float(nbins)) / float(nbins);
			float b = floor(color.b * float(nbins)) / float(nbins);
			color = vec4(r, g, b, color.a);

			vec2 sum = vec2(0.0);
			vec2 norm = vec2(0.0);
			int kernel_count = 0;
			for (int i = -halfWidth; i <= halfWidth; ++i) {

				for (int j = -halfWidth; j <= halfWidth; ++j) {

					float d = length(vec2(i, j));
					vec2 kernel = vec2(exp(-d * d / twoSigmaESquared),
						exp(-d * d / twoSigmaRSquared));
					vec4 c = texture(tex, fs_in.texcoord + vec2(i, j) / img_size);
					vec2 L = vec2(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);

					norm += 2.0 * kernel;
					sum += kernel * L;
				}
			}
			sum /= norm;

			float H = 100.0 * (sum.x - tau * sum.y);
			float edge = (H > 0.0) ? 1.0 : 2.0 * smoothstep(-2.0, 2.0, phi * H);

			color *= vec4(edge, edge, edge, 1.0);

			break;
		}
		case(8):
		{
			color = 1 -  texture(tex, fs_in.texcoord);
			break;
		}
		case(9):
		{
			color = texture(tex, fs_in.texcoord);
			float grayscale_col = 0.299*color.r + 0.587*color.g + 0.114*color.b;
			vec3 col = vec3(grayscale_col,grayscale_col,grayscale_col);
			color = vec4(col, 1.0);
			break;
		}
		case(10):
		{
			vec4 noise = vec4(gold_noise(fs_in.texcoord, iTime+1), // r
                gold_noise(fs_in.texcoord, iTime+2), // g
                gold_noise(fs_in.texcoord, iTime+3), // b
                1.0);
			color = texture(tex, fs_in.texcoord);
			color /= noise;
			break;
		}
		case(11):
		{
			

			float time = iTime*0.001f; //* .5+23.0;
			// uv should be the 0-1 uv of texture...
			vec2 uv = fs_in.texcoord;
			vec2 p = mod(uv*TAU, TAU)-250.0;

			vec2 i = vec2(p);
			float c = 1.0;
			float inten = .005;

			for (int n = 0; n < MAX_ITER; n++) 
			{
				float t = time * (1.0 - (3.5 / float(n+1)));
				i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
				c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
			}
			c /= float(MAX_ITER);
			c = 1.17-pow(c, 1.4);
			vec3 colour = vec3(pow(abs(c), 8.0));
			colour = clamp(colour + vec3(0.0, 0.35, 0.5), 0.0, 1.0);

			color = texture(tex, fs_in.texcoord);
			color += vec4(colour,1);

			break;
		}
		case(12):
		{
			

			float time = iTime*0.001f; //* .5+23.0;
			// uv should be the 0-1 uv of texture...
			vec2 uv = fs_in.texcoord;
			vec2 p = mod(uv*TAU, TAU)-250.0;

			vec2 i = vec2(p);
			float c = 1.0;
			float inten = .005;

			for (int n = 0; n < MAX_ITER; n++) 
			{
				float t = time * (1.0 - (3.5 / float(n+1)));
				i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
				c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
			}
			c /= float(MAX_ITER);
			c = 1.17-pow(c, 1.4);
			vec3 colour = vec3(pow(abs(c), 8.0));
			colour = clamp(colour + vec3(0.0, 0.35, 0.5), 0.0, 1.0);

			color = texture(tex, fs_in.texcoord);
			color /= vec4(colour,1);

			break;
		}
	}
}
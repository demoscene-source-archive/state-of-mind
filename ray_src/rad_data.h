/***********************************************
 *        Radiosity data                       *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

typedef struct { BYTE x, y, z; } BYTE_XYZ;
  
BYTE_XYZ Rad_Samples[1600] = {
	{ 250, 127, 71 },
	{ 116, 109, 251 },
	{ 146, 45, 191 },
	{ 110, 249, 71 },
	{ 196, 182, 186 },
	{ 178, 16, 71 },
	{ 80, 150, 232 },
	{ 9, 127, 94 },
	{ 147, 125, 252 },
	{ 205, 38, 94 },
	{ 217, 117, 181 },
	{ 241, 94, 94 },
	{ 113, 142, 252 },
	{ 231, 194, 71 },
	{ 43, 145, 187 },
	{ 170, 206, 183 },
	{ 172, 154, 233 },
	{ 111, 215, 182 },
	{ 78, 235, 94 },
	{ 78, 20, 94 },
	{ 163, 91, 234 },
	{ 110, 6, 71 },
	{ 10, 162, 71 },
	{ 41, 111, 185 },
	{ 208, 149, 194 },
	{ 57, 176, 190 },
	{ 96, 81, 229 },
	{ 138, 195, 215 },
	{ 59, 82, 195 },
	{ 241, 161, 94 },
	{ 47, 220, 71 },
	{ 106, 177, 231 },
	{ 178, 58, 189 },
	{ 47, 35, 71 },
	{ 178, 239, 71 },
	{ 131, 77, 234 },
	{ 201, 85, 190 },
	{ 144, 245, 94 },
	{ 144, 10, 94 },
	{ 80, 201, 185 },
	{ 183, 120, 229 },
	{ 28, 192, 94 },
	{ 10, 93, 71 },
	{ 77, 53, 181 },
	{ 28, 63, 94 },
	{ 140, 161, 245 },
	{ 111, 46, 193 },
	{ 78, 114, 234 },
	{ 231, 61, 71 },
	{ 219, 171, 156 },
	{ 205, 217, 94 },
	{ 155, 67, 218 },
	{ 35, 87, 155 },
	{ 229, 139, 153 },
	{ 155, 177, 229 },
	{ 90, 134, 244 },
	{ 47, 198, 137 },
	{ 24, 143, 145 },
	{ 150, 146, 248 },
	{ 111, 65, 220 },
	{ 89, 39, 168 },
	{ 98, 158, 241 },
	{ 187, 101, 219 },
	{ 104, 24, 141 },
	{ 222, 86, 150 },
	{ 117, 90, 243 },
	{ 78, 76, 211 },
	{ 60, 158, 208 },
	{ 129, 131, 255 },
	{ 179, 78, 211 },
	{ 65, 100, 216 },
	{ 197, 133, 214 },
	{ 186, 214, 146 },
	{ 121, 157, 248 },
	{ 104, 122, 250 },
	{ 65, 192, 181 },
	{ 145, 91, 242 },
	{ 175, 181, 212 },
	{ 128, 178, 234 },
	{ 208, 196, 143 },
	{ 138, 216, 183 },
	{ 137, 108, 251 },
	{ 119, 198, 212 },
	{ 54, 123, 208 },
	{ 34, 168, 154 },
	{ 191, 161, 211 },
	{ 159, 226, 149 },
	{ 129, 28, 160 },
	{ 204, 61, 156 },
	{ 124, 233, 142 },
	{ 67, 135, 224 },
	{ 23, 111, 142 },
	{ 52, 59, 154 },
	{ 168, 36, 158 },
	{ 81, 175, 218 },
	{ 67, 216, 137 },
	{ 92, 222, 157 },
	{ 92, 100, 239 },
	{ 168, 132, 241 },
	{ 247, 145, 82 },
	{ 97, 195, 207 },
	{ 251, 109, 50 },
	{ 152, 194, 212 },
	{ 60, 22, 50 },
	{ 241, 76, 50 },
	{ 17, 178, 80 },
	{ 191, 28, 97 },
	{ 215, 99, 176 },
	{ 161, 18, 112 },
	{ 17, 77, 78 },
	{ 236, 177, 87 },
	{ 218, 50, 90 },
	{ 183, 194, 187 },
	{ 106, 232, 138 },
	{ 127, 11, 103 },
	{ 172, 107, 235 },
	{ 234, 122, 140 },
	{ 194, 50, 154 },
	{ 155, 103, 244 },
	{ 40, 129, 186 },
	{ 97, 53, 199 },
	{ 141, 58, 212 },
	{ 162, 52, 193 },
	{ 44, 73, 158 },
	{ 123, 54, 208 },
	{ 60, 45, 141 },
	{ 73, 35, 139 },
	{ 42, 182, 155 },
	{ 156, 161, 240 },
	{ 154, 210, 187 },
	{ 25, 127, 153 },
	{ 49, 96, 190 },
	{ 217, 70, 141 },
	{ 214, 134, 186 },
	{ 230, 105, 144 },
	{ 193, 115, 217 },
	{ 170, 168, 226 },
	{ 164, 119, 244 },
	{ 160, 81, 228 },
	{ 142, 230, 148 },
	{ 181, 42, 156 },
	{ 174, 224, 138 },
	{ 205, 167, 187 },
	{ 144, 25, 147 },
	{ 219, 185, 135 },
	{ 89, 25, 131 },
	{ 23, 158, 134 },
	{ 124, 222, 171 },
	{ 195, 201, 158 },
	{ 129, 95, 247 },
	{ 128, 66, 223 },
	{ 142, 178, 232 },
	{ 70, 88, 213 },
	{ 123, 120, 254 },
	{ 154, 35, 166 },
	{ 98, 144, 246 },
	{ 96, 111, 245 },
	{ 24, 95, 133 },
	{ 183, 133, 230 },
	{ 91, 185, 216 },
	{ 128, 207, 200 },
	{ 114, 77, 233 },
	{ 64, 203, 161 },
	{ 186, 173, 207 },
	{ 77, 132, 234 },
	{ 179, 91, 221 },
	{ 131, 42, 189 },
	{ 103, 90, 238 },
	{ 72, 165, 217 },
	{ 84, 89, 227 },
	{ 169, 68, 210 },
	{ 78, 102, 230 },
	{ 109, 189, 221 },
	{ 194, 147, 214 },
	{ 55, 141, 208 },
	{ 162, 143, 243 },
	{ 118, 171, 239 },
	{ 109, 159, 244 },
	{ 205, 121, 202 },
	{ 142, 137, 253 },
	{ 91, 167, 231 },
	{ 87, 210, 176 },
	{ 148, 113, 250 },
	{ 73, 184, 202 },
	{ 180, 146, 229 },
	{ 78, 224, 134 },
	{ 125, 188, 224 },
	{ 108, 103, 247 },
	{ 131, 147, 252 },
	{ 203, 102, 200 },
	{ 103, 206, 194 },
	{ 51, 211, 116 },
	{ 144, 75, 230 },
	{ 88, 121, 242 },
	{ 224, 155, 156 },
	{ 4, 110, 50 },
	{ 114, 33, 170 },
	{ 90, 66, 211 },
	{ 47, 162, 185 },
	{ 133, 252, 50 },
	{ 165, 191, 208 },
	{ 3, 140, 48 },
	{ 86, 246, 42 },
	{ 160, 248, 49 },
	{ 66, 237, 42 },
	{ 33, 49, 67 },
	{ 67, 64, 185 },
	{ 156, 5, 42 },
	{ 36, 207, 81 },
	{ 200, 230, 45 },
	{ 86, 9, 42 },
	{ 202, 72, 175 },
	{ 66, 122, 223 },
	{ 54, 108, 205 },
	{ 187, 228, 102 },
	{ 212, 207, 104 },
	{ 99, 244, 88 },
	{ 46, 48, 115 },
	{ 12, 149, 100 },
	{ 166, 237, 106 },
	{ 136, 118, 254 },
	{ 189, 86, 208 },
	{ 123, 245, 97 },
	{ 228, 74, 112 },
	{ 102, 73, 225 },
	{ 98, 12, 93 },
	{ 180, 30, 127 },
	{ 59, 226, 86 },
	{ 115, 128, 254 },
	{ 168, 9, 42 },
	{ 40, 60, 127 },
	{ 30, 179, 127 },
	{ 208, 179, 169 },
	{ 153, 237, 121 },
	{ 230, 167, 130 },
	{ 91, 234, 121 },
	{ 191, 62, 178 },
	{ 33, 118, 170 },
	{ 14, 117, 116 },
	{ 205, 49, 127 },
	{ 192, 75, 194 },
	{ 54, 185, 173 },
	{ 32, 74, 131 },
	{ 38, 157, 172 },
	{ 67, 111, 222 },
	{ 220, 198, 104 },
	{ 31, 105, 161 },
	{ 61, 34, 109 },
	{ 246, 171, 42 },
	{ 188, 39, 138 },
	{ 105, 36, 172 },
	{ 165, 216, 168 },
	{ 103, 133, 250 },
	{ 223, 128, 169 },
	{ 127, 108, 252 },
	{ 169, 25, 126 },
	{ 56, 71, 177 },
	{ 14, 104, 107 },
	{ 130, 168, 242 },
	{ 63, 147, 217 },
	{ 234, 85, 112 },
	{ 140, 35, 173 },
	{ 243, 118, 104 },
	{ 214, 159, 176 },
	{ 79, 64, 199 },
	{ 135, 18, 130 },
	{ 143, 205, 200 },
	{ 47, 84, 178 },
	{ 37, 192, 126 },
	{ 153, 136, 249 },
	{ 212, 80, 166 },
	{ 149, 221, 168 },
	{ 24, 84, 120 },
	{ 240, 132, 120 },
	{ 120, 22, 143 },
	{ 240, 106, 111 },
	{ 209, 91, 182 },
	{ 85, 142, 239 },
	{ 123, 138, 254 },
	{ 47, 134, 198 },
	{ 106, 55, 205 },
	{ 208, 110, 194 },
	{ 198, 212, 131 },
	{ 90, 153, 238 },
	{ 64, 54, 166 },
	{ 79, 44, 166 },
	{ 16, 136, 121 },
	{ 55, 195, 160 },
	{ 140, 148, 250 },
	{ 183, 109, 227 },
	{ 104, 168, 237 },
	{ 147, 103, 247 },
	{ 10, 173, 42 },
	{ 82, 162, 228 },
	{ 115, 227, 158 },
	{ 172, 48, 178 },
	{ 94, 175, 227 },
	{ 46, 172, 174 },
	{ 204, 140, 202 },
	{ 148, 169, 238 },
	{ 227, 116, 158 },
	{ 39, 98, 173 },
	{ 181, 68, 198 },
	{ 112, 15, 116 },
	{ 223, 62, 107 },
	{ 228, 95, 143 },
	{ 228, 179, 120 },
	{ 159, 128, 247 },
	{ 182, 205, 171 },
	{ 83, 191, 202 },
	{ 174, 126, 237 },
	{ 74, 142, 229 },
	{ 67, 75, 199 },
	{ 74, 208, 167 },
	{ 103, 223, 163 },
	{ 108, 199, 207 },
	{ 68, 174, 205 },
	{ 193, 125, 219 },
	{ 172, 141, 237 },
	{ 201, 157, 200 },
	{ 98, 44, 185 },
	{ 144, 186, 224 },
	{ 118, 209, 195 },
	{ 196, 171, 196 },
	{ 136, 49, 200 },
	{ 160, 201, 198 },
	{ 212, 59, 133 },
	{ 51, 152, 198 },
	{ 196, 95, 205 },
	{ 71, 155, 222 },
	{ 171, 84, 223 },
	{ 165, 178, 222 },
	{ 98, 213, 180 },
	{ 162, 153, 240 },
	{ 221, 106, 169 },
	{ 58, 166, 199 },
	{ 120, 149, 251 },
	{ 107, 149, 248 },
	{ 189, 140, 222 },
	{ 46, 119, 195 },
	{ 130, 198, 213 },
	{ 100, 64, 214 },
	{ 134, 226, 161 },
	{ 184, 154, 222 },
	{ 137, 127, 254 },
	{ 234, 155, 130 },
	{ 88, 76, 220 },
	{ 115, 56, 210 },
	{ 140, 68, 224 },
	{ 237, 144, 126 },
	{ 58, 93, 203 },
	{ 59, 131, 214 },
	{ 137, 98, 247 },
	{ 91, 201, 194 },
	{ 80, 217, 156 },
	{ 87, 110, 239 },
	{ 74, 94, 222 },
	{ 173, 97, 231 },
	{ 100, 186, 220 },
	{ 83, 32, 142 },
	{ 173, 116, 237 },
	{ 152, 80, 232 },
	{ 31, 149, 162 },
	{ 191, 190, 182 },
	{ 77, 123, 234 },
	{ 120, 44, 192 },
	{ 159, 43, 181 },
	{ 148, 156, 245 },
	{ 156, 186, 219 },
	{ 131, 157, 248 },
	{ 124, 35, 175 },
	{ 154, 92, 239 },
	{ 123, 86, 241 },
	{ 202, 190, 165 },
	{ 94, 90, 234 },
	{ 156, 116, 248 },
	{ 120, 66, 223 },
	{ 159, 60, 207 },
	{ 173, 194, 197 },
	{ 207, 129, 199 },
	{ 101, 100, 243 },
	{ 163, 106, 241 },
	{ 197, 39, 119 },
	{ 122, 76, 233 },
	{ 156, 26, 142 },
	{ 141, 83, 238 },
	{ 131, 58, 214 },
	{ 110, 116, 252 },
	{ 117, 189, 222 },
	{ 117, 179, 232 },
	{ 108, 83, 236 },
	{ 164, 74, 219 },
	{ 85, 98, 233 },
	{ 57, 208, 139 },
	{ 179, 164, 221 },
	{ 33, 137, 170 },
	{ 119, 99, 248 },
	{ 218, 145, 175 },
	{ 96, 31, 155 },
	{ 213, 221, 36 },
	{ 133, 87, 242 },
	{ 250, 159, 36 },
	{ 19, 192, 36 },
	{ 212, 34, 36 },
	{ 18, 66, 43 },
	{ 222, 209, 57 },
	{ 10, 82, 36 },
	{ 247, 87, 36 },
	{ 38, 217, 36 },
	{ 253, 136, 36 },
	{ 152, 54, 202 },
	{ 250, 100, 48 },
	{ 69, 23, 86 },
	{ 31, 209, 36 },
	{ 135, 7, 82 },
	{ 181, 187, 198 },
	{ 188, 238, 45 },
	{ 149, 19, 128 },
	{ 136, 244, 103 },
	{ 101, 4, 36 },
	{ 57, 233, 36 },
	{ 43, 43, 88 },
	{ 18, 166, 103 },
	{ 119, 4, 65 },
	{ 101, 251, 36 },
	{ 196, 221, 106 },
	{ 179, 216, 153 },
	{ 5, 119, 66 },
	{ 237, 68, 58 },
	{ 69, 223, 121 },
	{ 185, 49, 165 },
	{ 187, 19, 60 },
	{ 5, 101, 45 },
	{ 4, 154, 39 },
	{ 30, 200, 79 },
	{ 237, 187, 53 },
	{ 70, 47, 161 },
	{ 163, 162, 235 },
	{ 111, 94, 244 },
	{ 71, 198, 181 },
	{ 200, 30, 76 },
	{ 115, 234, 137 },
	{ 134, 235, 138 },
	{ 226, 53, 62 },
	{ 59, 116, 214 },
	{ 188, 182, 196 },
	{ 138, 171, 239 },
	{ 212, 43, 87 },
	{ 17, 126, 126 },
	{ 235, 113, 133 },
	{ 242, 152, 100 },
	{ 84, 59, 196 },
	{ 15, 86, 86 },
	{ 59, 216, 123 },
	{ 65, 181, 195 },
	{ 234, 100, 129 },
	{ 115, 163, 243 },
	{ 228, 147, 151 },
	{ 55, 51, 143 },
	{ 133, 189, 223 },
	{ 32, 96, 156 },
	{ 153, 13, 99 },
	{ 35, 42, 36 },
	{ 86, 16, 93 },
	{ 176, 35, 145 },
	{ 162, 171, 230 },
	{ 78, 84, 219 },
	{ 221, 43, 36 },
	{ 36, 78, 148 },
	{ 24, 174, 115 },
	{ 174, 232, 112 },
	{ 203, 201, 143 },
	{ 28, 135, 158 },
	{ 105, 109, 248 },
	{ 48, 66, 156 },
	{ 135, 210, 194 },
	{ 210, 187, 154 },
	{ 97, 129, 247 },
	{ 209, 68, 156 },
	{ 151, 229, 147 },
	{ 39, 69, 142 },
	{ 106, 240, 110 },
	{ 23, 184, 92 },
	{ 53, 41, 114 },
	{ 49, 189, 158 },
	{ 24, 72, 101 },
	{ 231, 130, 150 },
	{ 84, 50, 183 },
	{ 166, 60, 202 },
	{ 175, 137, 236 },
	{ 20, 150, 130 },
	{ 238, 169, 98 },
	{ 222, 163, 156 },
	{ 177, 172, 217 },
	{ 44, 206, 112 },
	{ 56, 100, 204 },
	{ 115, 242, 110 },
	{ 22, 120, 143 },
	{ 246, 136, 94 },
	{ 51, 78, 178 },
	{ 183, 222, 129 },
	{ 149, 61, 213 },
	{ 163, 29, 145 },
	{ 97, 23, 134 },
	{ 196, 58, 165 },
	{ 200, 110, 207 },
	{ 38, 175, 154 },
	{ 91, 48, 186 },
	{ 124, 213, 188 },
	{ 158, 221, 162 },
	{ 213, 151, 184 },
	{ 98, 229, 143 },
	{ 220, 78, 144 },
	{ 82, 183, 210 },
	{ 84, 128, 240 },
	{ 42, 91, 175 },
	{ 66, 39, 138 },
	{ 216, 192, 130 },
	{ 151, 202, 201 },
	{ 146, 213, 186 },
	{ 190, 208, 154 },
	{ 76, 27, 119 },
	{ 73, 69, 199 },
	{ 48, 104, 193 },
	{ 74, 242, 36 },
	{ 72, 59, 185 },
	{ 216, 179, 153 },
	{ 131, 218, 180 },
	{ 123, 164, 244 },
	{ 219, 93, 163 },
	{ 110, 208, 195 },
	{ 168, 148, 239 },
	{ 188, 199, 173 },
	{ 202, 175, 184 },
	{ 83, 231, 119 },
	{ 128, 49, 200 },
	{ 77, 169, 219 },
	{ 127, 20, 136 },
	{ 16, 97, 107 },
	{ 185, 57, 179 },
	{ 46, 55, 133 },
	{ 120, 14, 114 },
	{ 53, 160, 197 },
	{ 170, 54, 191 },
	{ 162, 209, 184 },
	{ 31, 159, 155 },
	{ 122, 128, 255 },
	{ 152, 247, 77 },
	{ 45, 154, 188 },
	{ 54, 220, 97 },
	{ 59, 62, 170 },
	{ 112, 21, 136 },
	{ 72, 106, 226 },
	{ 93, 59, 203 },
	{ 105, 141, 250 },
	{ 167, 227, 140 },
	{ 138, 29, 162 },
	{ 143, 238, 125 },
	{ 181, 96, 223 },
	{ 35, 185, 132 },
	{ 135, 182, 230 },
	{ 23, 103, 137 },
	{ 162, 135, 245 },
	{ 26, 166, 135 },
	{ 226, 188, 106 },
	{ 212, 142, 189 },
	{ 10, 141, 95 },
	{ 33, 56, 96 },
	{ 89, 193, 205 },
	{ 50, 179, 174 },
	{ 160, 232, 129 },
	{ 160, 98, 239 },
	{ 128, 240, 120 },
	{ 159, 241, 95 },
	{ 148, 30, 159 },
	{ 195, 103, 210 },
	{ 148, 39, 178 },
	{ 174, 42, 165 },
	{ 71, 231, 96 },
	{ 104, 16, 115 },
	{ 93, 17, 109 },
	{ 153, 47, 190 },
	{ 137, 142, 253 },
	{ 211, 170, 173 },
	{ 212, 123, 191 },
	{ 141, 222, 168 },
	{ 33, 127, 171 },
	{ 222, 137, 171 },
	{ 114, 153, 248 },
	{ 178, 23, 105 },
	{ 53, 88, 191 },
	{ 96, 122, 247 },
	{ 60, 185, 183 },
	{ 83, 70, 210 },
	{ 197, 67, 177 },
	{ 155, 168, 235 },
	{ 95, 72, 220 },
	{ 182, 179, 207 },
	{ 143, 120, 253 },
	{ 118, 219, 177 },
	{ 130, 124, 255 },
	{ 111, 172, 237 },
	{ 104, 193, 213 },
	{ 108, 72, 227 },
	{ 132, 36, 176 },
	{ 205, 209, 121 },
	{ 136, 202, 206 },
	{ 41, 137, 186 },
	{ 148, 69, 223 },
	{ 112, 40, 183 },
	{ 188, 69, 191 },
	{ 204, 79, 179 },
	{ 139, 42, 187 },
	{ 72, 117, 228 },
	{ 115, 121, 253 },
	{ 191, 108, 218 },
	{ 234, 92, 120 },
	{ 130, 139, 254 },
	{ 175, 72, 208 },
	{ 188, 93, 214 },
	{ 63, 70, 187 },
	{ 60, 138, 215 },
	{ 155, 152, 244 },
	{ 70, 128, 228 },
	{ 195, 82, 197 },
	{ 167, 199, 196 },
	{ 124, 60, 216 },
	{ 161, 67, 214 },
	{ 123, 93, 245 },
	{ 36, 111, 174 },
	{ 214, 87, 169 },
	{ 96, 138, 246 },
	{ 148, 175, 233 },
	{ 88, 83, 226 },
	{ 209, 100, 189 },
	{ 200, 127, 209 },
	{ 179, 158, 225 },
	{ 184, 126, 229 },
	{ 217, 127, 182 },
	{ 61, 107, 214 },
	{ 19, 90, 112 },
	{ 75, 177, 210 },
	{ 92, 116, 244 },
	{ 108, 128, 252 },
	{ 164, 184, 216 },
	{ 52, 169, 188 },
	{ 153, 122, 249 },
	{ 171, 218, 158 },
	{ 66, 162, 212 },
	{ 33, 66, 119 },
	{ 197, 164, 201 },
	{ 126, 151, 251 },
	{ 174, 63, 200 },
	{ 51, 114, 202 },
	{ 114, 27, 155 },
	{ 102, 115, 249 },
	{ 246, 111, 90 },
	{ 190, 132, 222 },
	{ 40, 104, 179 },
	{ 116, 83, 238 },
	{ 188, 148, 221 },
	{ 175, 210, 170 },
	{ 227, 82, 129 },
	{ 171, 91, 228 },
	{ 196, 194, 168 },
	{ 48, 127, 199 },
	{ 64, 170, 204 },
	{ 241, 125, 114 },
	{ 207, 158, 189 },
	{ 203, 94, 195 },
	{ 90, 240, 91 },
	{ 81, 208, 174 },
	{ 130, 102, 250 },
	{ 60, 77, 191 },
	{ 201, 149, 204 },
	{ 235, 137, 135 },
	{ 135, 71, 229 },
	{ 162, 11, 78 },
	{ 85, 222, 149 },
	{ 65, 93, 212 },
	{ 90, 160, 235 },
	{ 219, 154, 169 },
	{ 82, 38, 157 },
	{ 85, 199, 193 },
	{ 34, 143, 170 },
	{ 57, 149, 208 },
	{ 199, 119, 211 },
	{ 88, 177, 221 },
	{ 171, 161, 230 },
	{ 224, 100, 158 },
	{ 181, 104, 227 },
	{ 166, 44, 177 },
	{ 137, 91, 244 },
	{ 171, 77, 218 },
	{ 71, 81, 208 },
	{ 186, 79, 205 },
	{ 41, 166, 170 },
	{ 150, 183, 225 },
	{ 172, 187, 207 },
	{ 129, 115, 254 },
	{ 104, 48, 193 },
	{ 197, 140, 212 },
	{ 137, 154, 249 },
	{ 155, 74, 225 },
	{ 212, 201, 122 },
	{ 156, 143, 247 },
	{ 187, 166, 213 },
	{ 28, 114, 158 },
	{ 202, 183, 175 },
	{ 149, 86, 237 },
	{ 212, 52, 116 },
	{ 85, 169, 226 },
	{ 91, 146, 241 },
	{ 66, 210, 151 },
	{ 94, 207, 188 },
	{ 97, 167, 235 },
	{ 61, 124, 217 },
	{ 105, 215, 179 },
	{ 114, 135, 253 },
	{ 99, 151, 244 },
	{ 38, 150, 177 },
	{ 161, 37, 166 },
	{ 151, 108, 248 },
	{ 167, 112, 241 },
	{ 234, 161, 122 },
	{ 89, 33, 153 },
	{ 223, 69, 120 },
	{ 164, 85, 229 },
	{ 67, 141, 223 },
	{ 80, 94, 227 },
	{ 111, 182, 228 },
	{ 112, 195, 215 },
	{ 143, 131, 253 },
	{ 167, 100, 236 },
	{ 202, 54, 145 },
	{ 120, 39, 182 },
	{ 102, 81, 232 },
	{ 104, 161, 242 },
	{ 159, 111, 245 },
	{ 123, 182, 230 },
	{ 91, 215, 169 },
	{ 209, 116, 194 },
	{ 117, 203, 204 },
	{ 176, 149, 232 },
	{ 114, 102, 249 },
	{ 222, 113, 169 },
	{ 81, 137, 237 },
	{ 142, 111, 251 },
	{ 224, 121, 166 },
	{ 96, 189, 214 },
	{ 146, 142, 251 },
	{ 135, 63, 220 },
	{ 83, 156, 232 },
	{ 99, 179, 226 },
	{ 117, 71, 228 },
	{ 134, 163, 244 },
	{ 21, 134, 141 },
	{ 105, 30, 158 },
	{ 82, 107, 235 },
	{ 124, 173, 238 },
	{ 62, 197, 169 },
	{ 42, 81, 165 },
	{ 198, 46, 135 },
	{ 135, 23, 146 },
	{ 104, 95, 242 },
	{ 100, 201, 201 },
	{ 43, 191, 143 },
	{ 116, 61, 216 },
	{ 178, 114, 232 },
	{ 89, 93, 234 },
	{ 241, 100, 101 },
	{ 135, 54, 208 },
	{ 49, 143, 198 },
	{ 29, 88, 140 },
	{ 107, 61, 214 },
	{ 170, 122, 240 },
	{ 151, 131, 250 },
	{ 83, 118, 239 },
	{ 128, 82, 238 },
	{ 65, 153, 216 },
	{ 214, 108, 183 },
	{ 148, 163, 242 },
	{ 182, 140, 229 },
	{ 116, 50, 201 },
	{ 90, 228, 139 },
	{ 187, 33, 123 },
	{ 164, 126, 244 },
	{ 72, 149, 226 },
	{ 177, 131, 235 },
	{ 124, 194, 217 },
	{ 126, 72, 229 },
	{ 41, 35, 36 },
	{ 151, 97, 243 },
	{ 176, 102, 231 },
	{ 188, 117, 224 },
	{ 156, 215, 176 },
	{ 94, 106, 242 },
	{ 137, 79, 235 },
	{ 145, 151, 248 },
	{ 143, 98, 246 },
	{ 253, 122, 39 },
	{ 49, 28, 30 },
	{ 173, 245, 43 },
	{ 208, 225, 30 },
	{ 2, 127, 47 },
	{ 140, 253, 30 },
	{ 125, 252, 57 },
	{ 218, 215, 46 },
	{ 94, 249, 44 },
	{ 52, 229, 30 },
	{ 193, 232, 67 },
	{ 27, 54, 53 },
	{ 95, 6, 39 },
	{ 226, 200, 72 },
	{ 14, 72, 30 },
	{ 52, 203, 140 },
	{ 72, 16, 53 },
	{ 250, 151, 57 },
	{ 75, 190, 196 },
	{ 53, 32, 81 },
	{ 194, 22, 54 },
	{ 129, 5, 68 },
	{ 227, 173, 132 },
	{ 242, 182, 30 },
	{ 254, 115, 30 },
	{ 167, 14, 85 },
	{ 6, 134, 78 },
	{ 27, 204, 30 },
	{ 22, 60, 47 },
	{ 15, 110, 114 },
	{ 117, 249, 74 },
	{ 16, 184, 47 },
	{ 141, 4, 57 },
	{ 30, 46, 30 },
	{ 181, 230, 107 },
	{ 5, 145, 64 },
	{ 11, 156, 88 },
	{ 40, 54, 112 },
	{ 162, 6, 30 },
	{ 136, 135, 254 },
	{ 145, 193, 216 },
	{ 173, 18, 96 },
	{ 240, 88, 91 },
	{ 217, 57, 115 },
	{ 80, 13, 59 },
	{ 166, 244, 73 },
	{ 202, 223, 79 },
	{ 80, 242, 61 },
	{ 17, 160, 109 },
	{ 211, 214, 85 },
	{ 184, 25, 100 },
	{ 40, 117, 183 },
	{ 125, 144, 253 },
	{ 129, 247, 91 },
	{ 64, 231, 80 },
	{ 47, 225, 30 },
	{ 16, 144, 118 },
	{ 5, 95, 30 },
	{ 9, 106, 82 },
	{ 227, 206, 30 },
	{ 198, 74, 184 },
	{ 217, 38, 31 },
	{ 120, 28, 158 },
	{ 188, 46, 153 },
	{ 64, 87, 206 },
	{ 85, 237, 99 },
	{ 225, 47, 30 },
	{ 47, 214, 96 },
	{ 206, 31, 56 },
	{ 184, 14, 30 },
	{ 232, 182, 95 },
	{ 191, 176, 199 },
	{ 173, 31, 138 },
	{ 5, 161, 30 },
	{ 183, 84, 213 },
	{ 172, 176, 219 },
	{ 103, 9, 79 },
	{ 57, 203, 150 },
	{ 57, 27, 69 },
	{ 102, 237, 119 },
	{ 232, 174, 112 },
	{ 150, 7, 71 },
	{ 63, 28, 93 },
	{ 139, 13, 111 },
	{ 63, 221, 115 },
	{ 209, 136, 196 },
	{ 67, 34, 123 },
	{ 36, 201, 101 },
	{ 7, 88, 31 },
	{ 121, 239, 123 },
	{ 139, 248, 79 },
	{ 73, 219, 141 },
	{ 159, 192, 211 },
	{ 229, 88, 133 },
	{ 27, 78, 122 },
	{ 236, 74, 84 },
	{ 178, 198, 188 },
	{ 183, 241, 30 },
	{ 124, 104, 251 },
	{ 95, 37, 168 },
	{ 132, 173, 238 },
	{ 190, 222, 117 },
	{ 177, 48, 173 },
	{ 192, 216, 131 },
	{ 142, 51, 203 },
	{ 242, 176, 57 },
	{ 30, 173, 137 },
	{ 183, 235, 82 },
	{ 127, 228, 157 },
	{ 236, 193, 30 },
	{ 9, 113, 90 },
	{ 39, 48, 92 },
	{ 141, 19, 132 },
	{ 27, 154, 148 },
	{ 147, 199, 208 },
	{ 248, 93, 51 },
	{ 41, 211, 86 },
	{ 217, 64, 131 },
	{ 78, 195, 192 },
	{ 241, 139, 114 },
	{ 115, 9, 93 },
	{ 202, 43, 120 },
	{ 203, 134, 205 },
	{ 193, 34, 113 },
	{ 181, 53, 177 },
	{ 122, 204, 204 },
	{ 68, 187, 192 },
	{ 208, 84, 178 },
	{ 37, 123, 179 },
	{ 10, 121, 100 },
	{ 60, 40, 127 },
	{ 155, 86, 235 },
	{ 81, 25, 121 },
	{ 31, 82, 140 },
	{ 193, 89, 205 },
	{ 144, 64, 218 },
	{ 73, 136, 229 },
	{ 91, 12, 78 },
	{ 172, 238, 89 },
	{ 232, 79, 108 },
	{ 100, 173, 232 },
	{ 222, 177, 141 },
	{ 84, 146, 237 },
	{ 95, 96, 238 },
	{ 133, 12, 106 },
	{ 226, 133, 162 },
	{ 89, 54, 194 },
	{ 103, 155, 244 },
	{ 119, 142, 253 },
	{ 70, 53, 173 },
	{ 218, 204, 96 },
	{ 106, 42, 185 },
	{ 82, 80, 219 },
	{ 224, 142, 164 },
	{ 60, 191, 174 },
	{ 225, 194, 96 },
	{ 54, 65, 166 },
	{ 216, 165, 167 },
	{ 210, 164, 181 },
	{ 246, 105, 80 },
	{ 50, 72, 168 },
	{ 151, 252, 30 },
	{ 109, 225, 159 },
	{ 188, 126, 224 },
	{ 199, 63, 168 },
	{ 120, 56, 210 },
	{ 107, 78, 231 },
	{ 238, 119, 125 },
	{ 93, 28, 144 },
	{ 40, 170, 164 },
	{ 28, 185, 112 },
	{ 231, 95, 132 },
	{ 152, 42, 184 },
	{ 42, 159, 179 },
	{ 250, 141, 62 },
	{ 102, 146, 247 },
	{ 167, 20, 111 },
	{ 100, 34, 165 },
	{ 84, 43, 170 },
	{ 129, 234, 140 },
	{ 109, 165, 241 },
	{ 22, 168, 119 },
	{ 156, 64, 213 },
	{ 156, 206, 193 },
	{ 117, 184, 228 },
	{ 200, 35, 98 },
	{ 129, 184, 229 },
	{ 50, 53, 138 },
	{ 172, 200, 190 },
	{ 159, 175, 228 },
	{ 249, 117, 77 },
	{ 137, 239, 123 },
	{ 52, 131, 205 },
	{ 79, 143, 234 },
	{ 56, 112, 209 },
	{ 220, 99, 167 },
	{ 134, 196, 215 },
	{ 202, 196, 156 },
	{ 109, 120, 252 },
	{ 73, 64, 192 },
	{ 162, 24, 131 },
	{ 178, 184, 205 },
	{ 45, 101, 186 },
	{ 127, 160, 246 },
	{ 164, 56, 198 },
	{ 19, 100, 123 },
	{ 72, 237, 72 },
	{ 157, 54, 199 },
	{ 97, 133, 247 },
	{ 72, 160, 220 },
	{ 48, 194, 149 },
	{ 207, 75, 169 },
	{ 76, 231, 106 },
	{ 63, 176, 197 },
	{ 86, 21, 113 },
	{ 184, 73, 202 },
	{ 56, 181, 183 },
	{ 234, 168, 115 },
	{ 35, 72, 136 },
	{ 196, 190, 175 },
	{ 95, 239, 107 },
	{ 124, 218, 180 },
	{ 104, 246, 81 },
	{ 156, 198, 205 },
	{ 40, 76, 154 },
	{ 129, 119, 254 },
	{ 213, 175, 164 },
	{ 178, 118, 233 },
	{ 148, 240, 113 },
	{ 125, 134, 255 },
	{ 100, 108, 246 },
	{ 134, 45, 195 },
	{ 28, 100, 149 },
	{ 60, 103, 210 },
	{ 36, 162, 164 },
	{ 146, 81, 234 },
	{ 44, 131, 192 },
	{ 62, 212, 137 },
	{ 97, 69, 218 },
	{ 150, 24, 143 },
	{ 65, 129, 222 },
	{ 149, 75, 229 },
	{ 17, 154, 114 },
	{ 129, 202, 207 },
	{ 223, 149, 163 },
	{ 67, 198, 174 },
	{ 56, 45, 133 },
	{ 126, 39, 183 },
	{ 66, 117, 222 },
	{ 115, 158, 246 },
	{ 86, 204, 185 },
	{ 47, 203, 126 },
	{ 195, 158, 208 },
	{ 55, 77, 185 },
	{ 242, 146, 107 },
	{ 146, 233, 140 },
	{ 70, 29, 112 },
	{ 168, 232, 123 },
	{ 222, 55, 92 },
	{ 20, 71, 76 },
	{ 110, 108, 250 },
	{ 177, 83, 218 },
	{ 69, 204, 167 },
	{ 53, 224, 73 },
	{ 207, 56, 138 },
	{ 12, 132, 109 },
	{ 99, 93, 239 },
	{ 192, 168, 205 },
	{ 130, 212, 190 },
	{ 131, 143, 253 },
	{ 164, 221, 157 },
	{ 86, 181, 216 },
	{ 178, 227, 124 },
	{ 33, 196, 104 },
	{ 243, 166, 75 },
	{ 207, 43, 104 },
	{ 108, 90, 240 },
	{ 159, 121, 247 },
	{ 84, 65, 204 },
	{ 41, 199, 122 },
	{ 185, 218, 138 },
	{ 165, 139, 243 },
	{ 75, 48, 169 },
	{ 88, 105, 238 },
	{ 141, 104, 249 },
	{ 246, 156, 78 },
	{ 136, 167, 242 },
	{ 166, 158, 235 },
	{ 160, 48, 188 },
	{ 58, 56, 159 },
	{ 89, 172, 226 },
	{ 145, 250, 61 },
	{ 17, 172, 91 },
	{ 24, 179, 107 },
	{ 133, 94, 246 },
	{ 183, 38, 145 },
	{ 207, 173, 178 },
	{ 29, 141, 159 },
	{ 188, 156, 216 },
	{ 206, 184, 165 },
	{ 250, 134, 69 },
	{ 114, 114, 252 },
	{ 101, 218, 171 },
	{ 13, 79, 55 },
	{ 79, 98, 228 },
	{ 157, 148, 245 },
	{ 227, 65, 99 },
	{ 74, 88, 217 },
	{ 206, 97, 191 },
	{ 22, 78, 103 },
	{ 143, 166, 241 },
	{ 160, 237, 114 },
	{ 92, 78, 224 },
	{ 65, 59, 176 },
	{ 90, 129, 244 },
	{ 186, 188, 192 },
	{ 68, 168, 211 },
	{ 42, 176, 162 },
	{ 199, 79, 188 },
	{ 108, 135, 252 },
	{ 84, 214, 165 },
	{ 27, 119, 156 },
	{ 241, 113, 114 },
	{ 71, 42, 152 },
	{ 118, 167, 242 },
	{ 26, 148, 149 },
	{ 118, 214, 187 },
	{ 127, 32, 169 },
	{ 94, 155, 240 },
	{ 65, 81, 201 },
	{ 212, 191, 142 },
	{ 241, 71, 30 },
	{ 229, 122, 154 },
	{ 164, 64, 208 },
	{ 159, 87, 233 },
	{ 146, 15, 115 },
	{ 68, 148, 222 },
	{ 158, 245, 76 },
	{ 204, 115, 203 },
	{ 71, 213, 153 },
	{ 73, 100, 224 },
	{ 79, 205, 178 },
	{ 108, 12, 100 },
	{ 136, 123, 254 },
	{ 125, 125, 255 },
	{ 54, 95, 197 },
	{ 125, 25, 151 },
	{ 119, 133, 254 },
	{ 50, 121, 201 },
	{ 121, 110, 252 },
	{ 231, 111, 146 },
	{ 186, 63, 187 },
	{ 236, 107, 129 },
	{ 141, 199, 209 },
	{ 236, 128, 134 },
	{ 61, 97, 209 },
	{ 36, 180, 144 },
	{ 31, 92, 150 },
	{ 226, 162, 145 },
	{ 119, 230, 150 },
	{ 177, 191, 198 },
	{ 12, 168, 74 },
	{ 81, 168, 223 },
	{ 169, 193, 202 },
	{ 148, 50, 198 },
	{ 92, 86, 230 },
	{ 206, 145, 198 },
	{ 200, 216, 113 },
	{ 142, 155, 247 },
	{ 110, 237, 125 },
	{ 60, 66, 178 },
	{ 150, 189, 219 },
	{ 176, 204, 179 },
	{ 181, 46, 165 },
	{ 8, 151, 75 },
	{ 239, 81, 84 },
	{ 64, 141, 219 },
	{ 78, 220, 145 },
	{ 166, 210, 179 },
	{ 34, 132, 174 },
	{ 182, 169, 215 },
	{ 73, 226, 122 },
	{ 133, 129, 255 },
	{ 214, 103, 180 },
	{ 86, 136, 241 },
	{ 174, 167, 224 },
	{ 190, 55, 168 },
	{ 148, 35, 170 },
	{ 156, 231, 139 },
	{ 195, 129, 217 },
	{ 77, 119, 234 },
	{ 77, 163, 223 },
	{ 105, 182, 226 },
	{ 198, 114, 211 },
	{ 120, 62, 218 },
	{ 122, 177, 235 },
	{ 102, 58, 208 },
	{ 168, 31, 145 },
	{ 160, 181, 222 },
	{ 56, 135, 210 },
	{ 46, 60, 144 },
	{ 146, 181, 229 },
	{ 70, 192, 187 },
	{ 103, 85, 236 },
	{ 76, 128, 233 },
	{ 124, 114, 254 },
	{ 214, 112, 185 },
	{ 97, 49, 191 },
	{ 129, 16, 122 },
	{ 120, 224, 165 },
	{ 208, 205, 124 },
	{ 168, 41, 168 },
	{ 59, 199, 160 },
	{ 152, 164, 239 },
	{ 96, 202, 197 },
	{ 78, 33, 139 },
	{ 178, 220, 143 },
	{ 64, 49, 156 },
	{ 180, 129, 232 },
	{ 161, 102, 241 },
	{ 181, 210, 161 },
	{ 94, 181, 222 },
	{ 100, 164, 239 },
	{ 79, 180, 211 },
	{ 173, 132, 238 },
	{ 79, 187, 204 },
	{ 136, 147, 251 },
	{ 213, 146, 186 },
	{ 238, 156, 112 },
	{ 245, 130, 99 },
	{ 167, 95, 233 },
	{ 88, 45, 178 },
	{ 136, 39, 183 },
	{ 118, 193, 217 },
	{ 142, 144, 251 },
	{ 200, 169, 192 },
	{ 168, 107, 239 },
	{ 130, 72, 229 },
	{ 97, 233, 129 },
	{ 139, 25, 150 },
	{ 217, 150, 176 },
	{ 227, 127, 158 },
	{ 17, 121, 127 },
	{ 224, 169, 146 },
	{ 113, 188, 222 },
	{ 131, 52, 205 },
	{ 19, 131, 132 },
	{ 137, 230, 150 },
	{ 159, 31, 155 },
	{ 29, 70, 113 },
	{ 232, 69, 89 },
	{ 55, 213, 121 },
	{ 110, 34, 169 },
	{ 79, 89, 223 },
	{ 74, 23, 100 },
	{ 139, 75, 231 },
	{ 76, 153, 228 },
	{ 48, 90, 185 },
	{ 120, 153, 249 },
	{ 121, 188, 224 },
	{ 152, 172, 233 },
	{ 206, 191, 155 },
	{ 157, 133, 248 },
	{ 200, 144, 207 },
	{ 100, 209, 188 },
	{ 102, 127, 250 },
	{ 116, 44, 191 },
	{ 151, 125, 251 },
	{ 190, 81, 202 },
	{ 166, 80, 224 },
	{ 197, 153, 208 },
	{ 73, 172, 213 },
	{ 35, 61, 112 },
	{ 64, 74, 193 },
	{ 199, 50, 144 },
	{ 146, 56, 207 },
	{ 97, 102, 242 },
	{ 76, 81, 213 },
	{ 155, 20, 124 },
	{ 175, 77, 214 },
	{ 120, 88, 242 },
	{ 198, 89, 198 },
	{ 45, 139, 193 },
	{ 51, 101, 197 },
	{ 40, 186, 143 },
	{ 135, 103, 250 },
	{ 175, 26, 121 },
	{ 143, 39, 182 },
	{ 86, 151, 236 },
	{ 102, 52, 199 },
	{ 74, 202, 178 },
	{ 217, 140, 180 },
	{ 84, 226, 136 },
	{ 86, 187, 209 },
	{ 51, 207, 128 },
	{ 212, 73, 157 },
	{ 56, 36, 105 },
	{ 51, 47, 124 },
	{ 158, 14, 99 },
	{ 89, 99, 236 },
	{ 97, 117, 247 },
	{ 104, 188, 219 },
	{ 92, 124, 245 },
	{ 175, 160, 227 },
	{ 132, 152, 250 },
	{ 114, 147, 250 },
	{ 153, 224, 158 },
	{ 27, 161, 141 },
	{ 92, 190, 211 },
	{ 117, 18, 129 },
	{ 103, 198, 207 },
	{ 62, 134, 219 },
	{ 122, 8, 91 },
	{ 198, 124, 213 },
	{ 144, 218, 177 },
	{ 23, 190, 75 },
	{ 81, 131, 238 },
	{ 162, 148, 242 },
	{ 195, 207, 147 },
	{ 121, 81, 237 },
	{ 40, 86, 166 },
	{ 214, 93, 175 },
	{ 159, 165, 235 },
	{ 192, 120, 219 },
	{ 143, 173, 236 },
	{ 136, 176, 235 },
	{ 119, 161, 245 },
	{ 106, 114, 250 },
	{ 218, 88, 162 },
	{ 212, 130, 191 },
	{ 85, 195, 199 },
	{ 82, 55, 189 },
	{ 218, 122, 179 },
	{ 52, 198, 150 },
	{ 78, 59, 191 },
	{ 215, 197, 123 },
	{ 184, 161, 219 },
	{ 184, 90, 216 },
	{ 182, 150, 226 },
	{ 145, 86, 239 },
	{ 232, 143, 142 },
	{ 156, 39, 175 },
	{ 98, 224, 156 },
	{ 191, 103, 215 },
	{ 20, 140, 134 },
	{ 38, 142, 180 },
	{ 109, 98, 246 },
	{ 91, 140, 243 },
	{ 27, 107, 150 },
	{ 203, 162, 194 },
	{ 169, 182, 215 },
	{ 134, 31, 166 },
	{ 112, 124, 253 },
	{ 192, 143, 218 },
	{ 100, 40, 177 },
	{ 88, 219, 158 },
	{ 128, 89, 243 },
	{ 119, 124, 254 },
	{ 196, 108, 212 },
	{ 232, 134, 146 },
	{ 96, 218, 169 },
	{ 221, 190, 119 },
	{ 215, 184, 148 },
	{ 54, 190, 167 },
	{ 162, 114, 244 },
	{ 106, 172, 235 },
	{ 129, 62, 219 },
	{ 59, 144, 212 },
	{ 55, 118, 209 },
	{ 141, 210, 192 },
	{ 134, 83, 238 },
	{ 219, 159, 166 },
	{ 232, 188, 83 },
	{ 223, 74, 131 },
	{ 30, 123, 165 },
	{ 32, 111, 165 },
	{ 120, 32, 169 },
	{ 87, 28, 138 },
	{ 214, 47, 96 },
	{ 34, 154, 164 },
	{ 153, 242, 102 },
	{ 150, 65, 218 },
	{ 205, 89, 188 },
	{ 150, 234, 132 },
	{ 83, 124, 239 },
	{ 109, 50, 199 },
	{ 205, 107, 199 },
	{ 52, 174, 184 },
	{ 173, 111, 236 },
	{ 22, 125, 142 },
	{ 117, 95, 246 },
	{ 78, 70, 205 },
	{ 62, 112, 217 },
	{ 87, 158, 234 },
	{ 52, 164, 192 },
	{ 40, 40, 64 },
	{ 107, 67, 220 },
	{ 46, 78, 169 },
	{ 176, 39, 155 },
	{ 67, 105, 220 },
	{ 193, 43, 138 },
	{ 224, 183, 126 },
	{ 174, 121, 237 },
	{ 175, 88, 223 },
	{ 43, 123, 190 },
	{ 165, 166, 231 },
	{ 170, 212, 171 },
	{ 135, 112, 253 },
	{ 185, 96, 219 },
	{ 94, 149, 242 },
	{ 229, 152, 145 },
	{ 192, 196, 171 },
	{ 160, 213, 177 },
	{ 152, 71, 224 },
	{ 182, 34, 135 },
	{ 225, 105, 157 },
	{ 99, 19, 119 },
	{ 56, 127, 211 },
	{ 112, 71, 226 },
	{ 106, 211, 188 },
	{ 53, 83, 187 },
	{ 209, 63, 147 },
	{ 148, 136, 251 },
	{ 160, 157, 239 },
	{ 153, 58, 208 },
	{ 121, 49, 201 },
	{ 115, 37, 179 },
	{ 153, 157, 243 },
	{ 76, 39, 153 },
	{ 46, 178, 168 },
	{ 95, 170, 231 },
	{ 42, 153, 182 },
	{ 77, 108, 231 },
	{ 116, 175, 236 },
	{ 124, 169, 241 },
	{ 111, 204, 201 },
	{ 223, 81, 139 },
	{ 153, 30, 155 },
	{ 176, 94, 226 },
	{ 19, 114, 131 },
	{ 192, 66, 183 },
	{ 101, 139, 248 },
	{ 107, 19, 127 },
	{ 47, 110, 195 },
	{ 237, 150, 124 },
	{ 167, 173, 225 },
	{ 206, 125, 202 },
	{ 146, 108, 249 },
	{ 110, 244, 97 },
	{ 63, 164, 207 },
	{ 113, 221, 170 },
	{ 140, 184, 227 },
	{ 229, 158, 140 },
	{ 177, 143, 233 },
	{ 224, 91, 150 },
	{ 92, 197, 201 },
	{ 104, 104, 246 },
	{ 213, 119, 189 },
	{ 73, 75, 205 },
	{ 150, 216, 179 },
	{ 200, 57, 156 },
	{ 192, 185, 188 },
	{ 48, 41, 99 },
	{ 156, 80, 230 },
	{ 32, 189, 116 },
	{ 111, 59, 213 },
	{ 148, 119, 251 },
	{ 47, 148, 193 },
	{ 198, 186, 177 },
	{ 109, 144, 250 },
	{ 37, 92, 165 },
	{ 80, 49, 176 },
	{ 77, 212, 161 },
	{ 183, 200, 178 },
	{ 226, 110, 158 },
	{ 151, 102, 245 },
	{ 192, 97, 212 },
	{ 79, 228, 121 },
	{ 111, 231, 146 },
	{ 143, 71, 226 },
	{ 112, 87, 240 },
	{ 193, 137, 218 },
	{ 69, 97, 218 },
	{ 49, 157, 192 },
	{ 137, 59, 214 },
	{ 181, 174, 212 },
	{ 168, 143, 240 },
	{ 209, 48, 115 },
	{ 139, 87, 241 },
	{ 178, 153, 228 },
	{ 28, 130, 161 },
	{ 144, 160, 244 },
	{ 216, 76, 153 },
	{ 169, 222, 150 },
	{ 207, 200, 137 },
	{ 197, 177, 190 },
	{ 180, 63, 194 },
	{ 77, 158, 227 },
	{ 44, 95, 181 },
	{ 111, 177, 232 },
	{ 24, 90, 129 },
	{ 98, 59, 206 },
	{ 123, 209, 195 },
	{ 125, 99, 248 },
	{ 237, 97, 114 },
	{ 100, 28, 149 },
	{ 125, 44, 193 },
	{ 55, 155, 203 },
	{ 73, 112, 228 },
	{ 167, 153, 237 },
	{ 14, 91, 93 },
	{ 151, 151, 246 },
	{ 133, 206, 200 },
	{ 51, 138, 203 },
	{ 130, 223, 170 },
	{ 119, 116, 253 },
	{ 98, 86, 234 },
	{ 154, 182, 225 },
	{ 127, 78, 235 },
	{ 136, 159, 247 },
	{ 60, 152, 211 },
	{ 128, 55, 210 },
	{ 142, 125, 253 },
	{ 149, 91, 241 },
	{ 95, 162, 237 },
	{ 155, 127, 249 },
	{ 133, 67, 224 },
	{ 169, 137, 240 },
	{ 191, 152, 215 },
	{ 193, 71, 187 },
	{ 163, 131, 245 },
	{ 59, 88, 199 },
	{ 158, 93, 237 },
	{ 114, 199, 209 },
	{ 89, 61, 203 },
	{ 43, 65, 144 },
	{ 143, 31, 163 },
	{ 146, 226, 158 },
	{ 104, 228, 150 },
	{ 86, 164, 230 },
	{ 175, 53, 184 },
	{ 103, 69, 221 },
	{ 48, 184, 163 },
	{ 84, 102, 234 },
	{ 200, 206, 139 },
	{ 72, 123, 229 },
	{ 31, 164, 149 },
	{ 229, 100, 145 },
	{ 122, 70, 228 },
	{ 62, 207, 150 },
	{ 119, 105, 250 },
	{ 170, 102, 235 },
	{ 184, 183, 201 },
	{ 108, 219, 172 },
	{ 199, 98, 202 },
	{ 124, 200, 210 },
	{ 42, 217, 66 },
	{ 167, 50, 187 },
	{ 58, 161, 202 },
	{ 151, 141, 249 },
	{ 141, 94, 244 },
	{ 187, 194, 182 },
	{ 232, 117, 144 },
	{ 175, 214, 161 },
	{ 38, 134, 181 },
	{ 35, 101, 166 },
	{ 177, 108, 231 },
	{ 89, 71, 215 },
	{ 92, 110, 242 },
	{ 88, 116, 241 },
	{ 169, 127, 241 },
	{ 204, 153, 197 },
	{ 183, 115, 228 },
	{ 109, 153, 247 },
	{ 154, 112, 248 },
	{ 186, 136, 226 },
	{ 70, 179, 203 },
	{ 147, 131, 252 },
	{ 167, 88, 229 },
	{ 139, 190, 221 },
	{ 46, 167, 179 },
	{ 159, 71, 219 },
	{ 129, 192, 220 },
	{ 148, 208, 194 },
	{ 185, 144, 225 },
	{ 109, 26, 150 },
	{ 170, 72, 214 },
	{ 83, 112, 237 },
	{ 136, 221, 173 },
	{ 98, 77, 226 },
	{ 168, 117, 241 },
	{ 217, 82, 157 },
	{ 95, 64, 211 },
	{ 161, 76, 224 },
	{ 66, 157, 216 },
	{ 111, 54, 205 },
	{ 186, 106, 222 },
	{ 188, 112, 222 },
	{ 179, 124, 233 },
	{ 221, 117, 172 }
};

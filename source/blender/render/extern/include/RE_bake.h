/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2010 Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s):
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file RE_bake.h
 *  \ingroup render
 */

#ifndef __RE_BAKE_H__
#define __RE_BAKE_H__

struct Render;
struct Mesh;

typedef struct BakeImage {
	struct Image *image;
	int width;
	int height;
	int offset;
} BakeImage;

typedef struct BakePixel {
	int primitive_id;
	float uv[2];
	float du_dx, du_dy;
	float dv_dx, dv_dy;
} BakePixel;

typedef struct BakeHighPolyData {
	struct BakePixel *pixel_array;
	struct Object *ob;
	struct ModifierData *tri_mod;
	struct Mesh *me;
	char restrict_flag;
	float mat_lowtohigh[4][4];
} BakeHighPolyData;

/* external_engine.c */
bool RE_engine_has_bake(struct Render *re);

bool RE_engine_bake(struct Render *re, struct Object *object, const BakePixel pixel_array[], const int num_pixels, const int depth, const ScenePassType pass_type, float result[]);

/* bake.c */
int RE_pass_depth(const ScenePassType pass_type);
bool RE_internal_bake(struct Render *re, struct Object *object, const BakePixel pixel_array[], const int num_pixels, const int depth, const ScenePassType pass_type, float result[]);


void RE_mask_bake_pixels(const BakePixel pixel_array_from[],
                         BakePixel pixel_array_to[],
                         const int num_pixels);

void RE_populate_bake_pixels_from_objects(struct Mesh *me_low, BakePixel pixel_array_from[],
                                          BakeHighPolyData highpoly[], const int tot_highpoly, const int num_pixels,
                                          const float cage_extrusion);

void RE_populate_bake_pixels(struct Mesh *me, struct BakePixel *pixel_array, const int num_pixels, const struct BakeImage *images);

void RE_bake_margin(const BakePixel pixel_array[], struct ImBuf *ibuf, const int margin, const int width, const int height);

void RE_normal_world_to_object(const BakePixel pixel_array[], const int num_pixels, const int depth, float result[], struct Object *ob, const BakeNormalSwizzle normal_swizzle[3]);
void RE_normal_world_to_tangent(const BakePixel pixel_array[], const int num_pixels, const int depth, float result[], struct Mesh *me, const BakeNormalSwizzle normal_swizzle[3]);
void RE_normal_world_to_world(const BakePixel pixel_array[], const int num_pixels, const int depth, float result[], const BakeNormalSwizzle normal_swizzle[3]);

void RE_bake_ibuf_clear(struct BakeImage *images, const int tot_mat, const bool is_tangent);

#endif

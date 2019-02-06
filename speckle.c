/******************************************************************************
 'Speckle' with the current brush and color on the current drawable.
  Size and color can be varied by pseudo random.
******************************************************************************/

#include <libgimp/gimp.h>

/* Declaration of the mandatory plugin functions */

static void query (void);
static void run (	const gchar *name, gint nparams,	const GimpParam *param,	gint *nreturn_vals, GimpParam **return_vals);

/* This structure declaration is also mandatory */

GimpPlugInInfo PLUG_IN_INFO = {
	NULL,		//init, optional - called each time gimp starts |
	NULL,		//quit, optional - called before closing gimp   |> very rarely used
	query,	// mandatory pointer to query function
	run		// mandatory pointer to run function
};

/* Needed 'magic' C-Macro */

MAIN()

/*******************************************************************************
 * query: registration and input arguments definition.
 */

static void query (void) {

	static GimpParamDef args[] = {
		{ GIMP_PDB_INT32, "run-mode", "Run mode"},
		{ GIMP_PDB_IMAGE, "image", "Input image"},
		{ GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
		{ GIMP_PDB_INT32, "numSpeckles", "Number of speckles"},
		{ GIMP_PDB_INT32, "sizeVar", "Variation of max. size in pixels"},
		{ GIMP_PDB_INT32, "colVar", "Variation in brightness (0-255)"},
		{ GIMP_PDB_INT32, "seed", "Random seed"}
	}; // of GimpParamDerf args[] = ...

	gimp_install_procedure (
		"plug-in-speckle",									// func. name
		"Speckle!",									// window title
		"Speckle drawable with given brush and color",	// plugin description
		"Uwe Hoffmann",										// author
		"Copyright Uwe Hoffmann",						// copyright notice
		"2019",												// date created
		"_Speckle",								// menu entry name
		"RGB*, GRAY*",										// image types to work with
		GIMP_PLUGIN,										// declare this program as external plugin(NOT in core)
		G_N_ELEMENTS (args), 0,
		args, NULL
	);

	gimp_plugin_menu_register ("plug-in-speckle","<Image>/Uwes-Plugins");

} //static void query(void)...


static void run (	const gchar *name, gint nparams,	const GimpParam *param,	gint *nreturn_vals, GimpParam **return_vals) {

	static GimpParam  values[1];
	GimpPDBStatusType status = GIMP_PDB_SUCCESS;
	GimpRunMode		run_mode;
	GimpDrawable	*drawable;
	GimpRGB			itemRGB, startRGB, workRGB;
	guint32			drawableHeight,  drawableWidth,
						imageID, drawableID,
						starX, starY, seed, numSpeckles, sizeVar;
	gfloat starCol;
	int i;
	gdouble points[2];
	gdouble brushSize;

	/* Setting mandatory output values */
	*nreturn_vals = 1;
	*return_vals  = values;

	values[0].type = GIMP_PDB_STATUS;
	values[0].data.d_status = status;

	/* Getting run_mode - we won't display a dialog if
	* we are in NONINTERACTIVE mode */
	run_mode = param[0].data.d_int32;
	imageID  = param[1].data.d_int32;
	drawableID = param[2].data.d_int32;
	numSpeckles = param[3].data.d_int32;
	seed = param[4].data.d_int32;

	drawableWidth = gimp_drawable_width(drawableID);
	drawableHeight = gimp_drawable_height(drawableID);

	/* Begin the main actions context */

	gimp_context_push();
	gimp_image_undo_group_start(imageID);
	
	srand(seed);
	gimp_context_get_foreground(&startRGB);
	brushSize = gimp_context_get_brush_size();

	for (i = 0; i<500;i++) {
		starX = rand() % drawableWidth;
		starY = rand() % drawableHeight;
		starCol = rand() % 128/128.0;
		itemRGB = startRGB;
		gimp_rgb_set(&workRGB, starCol, starCol, starCol);
		gimp_rgb_subtract(&itemRGB, &workRGB);
		points[0] = starX;
		points[1] = starY;
		gimp_context_set_foreground(&itemRGB);
		gimp_context_set_brush_size(brushSize + rand() % 44);
		gimp_paintbrush_default(drawableID, 2, points);
	}

	gimp_image_undo_group_end(imageID);
	
	gimp_context_pop();
	gimp_displays_flush ();

	/* End of main actions context */

//	gimp_drawable_detach (drawableID);

/*	if (run_mode != GIMP_RUN_NONINTERACTIVE) {
		gimp_message_set_handler(GIMP_MESSAGE_BOX);
		gimp_message("Hello, world3!\n");
	}*/

} // of static void run ...


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
		{ GIMP_PDB_COLOR, "fgcolor",  "Color to draw with"}
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
	GimpRunMode       run_mode;
	GimpDrawable *drawable;
	GimpRGB itemRGB;
	gint32 drawableHeight;
	gint32 drawableWidth;
	gint32 imageID;
	gint32 drawableID;
	gint32 starX;
	gint32 starY;
	gdouble starCol;
	int i;
	gdouble points[2];

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

	drawableWidth = gimp_drawable_width(drawableID);
	drawableHeight = gimp_drawable_height(drawableID);

	gimp_context_push();
	gimp_image_undo_group_start(imageID);

	for (i = 0; i<300;i++) {
		starX = rand() % drawableWidth;
		starY = rand() % drawableHeight;
		starCol = 96 + rand() % 160;
		gimp_rgb_set(&itemRGB, starCol/255.0, starCol/255.0, starCol/255.0);
		points[0] = starX;
		points[1] = starY;
		gimp_context_set_foreground(&itemRGB);
		gimp_context_set_brush_size(10 + rand() % 44);
		gimp_paintbrush_default(drawableID, 2, points);
	}

	gimp_image_undo_group_end(imageID);
	
	gimp_context_pop();
	gimp_displays_flush ();
//	gimp_drawable_detach (drawableID);

/*	if (run_mode != GIMP_RUN_NONINTERACTIVE) {
		gimp_message_set_handler(GIMP_MESSAGE_BOX);
		gimp_message("Hello, world3!\n");
	}*/

} // of static void run ...


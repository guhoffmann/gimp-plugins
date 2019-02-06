#include <libgimp/gimp.h>

/* Declaration of the mandatory plugin functions */

static void query (void);
static void run (	const gchar *name, gint nparams,	const GimpParam *param,	gint *nreturn_vals, GimpParam **return_vals);

/* This structure declaration is also mandatory */

GimpPlugInInfo PLUG_IN_INFO = {
	NULL,		//init, optional - called each time gimp starts |
	NULL,		//quit, optional - called before closing gimp   |> both are very rarely used
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
		{
		  GIMP_PDB_INT32,
		  "run-mode",
		  "Run mode"
		},
		{
		  GIMP_PDB_IMAGE,
		  "image",
		  "Input image"
		},
		{
		  GIMP_PDB_DRAWABLE,
		  "drawable",
		  "Input drawable"
		}
	}; // of GimpParamDerf args[] = ...

	gimp_install_procedure (
		"plug-in-speckle",									// func. name
		"Speckle!",									// window title
		"Displays \"Hello, world!\" in a dialog",	// plugin description
		"David Neary",										// author
		"Copyright David Neary",						// copyright notice
		"2004",												// date created
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
	gint32 imageHeight;
	gint32 imageWidth;
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
	
	imageWidth = gimp_drawable_width(imageID);
	imageHeight = gimp_drawable_height(imageID);

	gimp_context_push();
	gimp_image_undo_group_start(imageID);

/*	drawableID = gimp_layer_new(imageID,"StarLayer",imageWidth,imageHeight,GIMP_RGB_IMAGE,100,0);
	gimp_image_insert_layer(imageID, drawableID, 0, 0);*/
	drawableID = param[2].data.d_int32;

	gimp_context_set_brush("2. Hardness 100");

	for (i = 0; i<1111;i++) {
		starX = rand() % imageWidth;
		starY = rand() % imageHeight;
		starCol = 96 + rand() % 160;
		gimp_rgb_set(&itemRGB, starCol/255.0, starCol/255.0, starCol/255.0);
		points[0] = starX;
		points[1] = starY;
		gimp_context_set_foreground(&itemRGB);
		gimp_context_set_brush_size(1 + rand() % 4);
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


/*  Get the specified drawable  */
	//drawable = gimp_drawable_get (param[2].data.d_drawable);
	//gimp_progress_init ("My stars...");

	/* Let's time blur
	*   GTimer timer = g_timer_new time ();
	*/

	//blur (drawable);

	/*   g_print ("blur() took %g seconds.\n", g_timer_elapsed (timer));
	*   g_timer_destroy (timer);
	*/

	//gimp_displays_flush ();
	//gimp_drawable_detach (drawable);
} // of static void run ...


#ifndef GATHRER_H 
/* --- Initialization ---- 
Initializes the gather library and loads all DLLs. */
typedef void GatherSession;


/* --- Initialization ---- 
Initializes the gather library and loads all DLLs. */

extern void		gather_init(void); /* Starts Gather. */
extern void		gather_format_count_get(void); /* Returns the number of file formats supported by gather */
extern char		*gather_format_name(void); /* Returns the name of the format. */
extern boolean	gather_format_save(void); /* Returns TRUE if the format can be saved. */
extern boolean	gather_format_load(void); /* Returns TRUE if the format can be loaded. */
extern boolean	gather_format_image(void); /* Returns TRUE if the format can store images. */
extern boolean	gather_format_audio(void); /* Returns TRUE if the format can store audio. */
extern boolean	gather_format_video(void); /* Returns TRUE if the format can store video. */

/* --- File loading ---- 
Initializes the gather library and loads all DLLs. */

extern GatherSession *gather_session_file_read_create(char *file_name); /* Opens a session by reading form a file. will return NULL if the file cant be open or the format is not understood by any plugin.*/
extern void		gather_session_destroy(GatherSession *session); /* Destropy a open session. */

/* --- Time ---- 
Retrive image data from a gather session. */

extern double	gather_session_duration_get(GatherSession *session); /* Returns the length of the media */
extern void		gather_session_seek(GatherSession *session, double time); /* */

/* --- Image ---- 
Retrive image data from a gather session. */

extern uint		gather_session_image_stream_count_get(GatherSession *session); /* Returns the number of image streams found in a session. */
extern void		gather_session_image_stream_active(GatherSession *session, uint stream_id, boolean active); /* activate or disable the stream so that its readable. */
extern void		gather_session_image_stream_size(GatherSession *session, uint stream_id, uint *x, uint *y); /* Writes out the X and Z resolution of the image stream*/
extern double	gather_session_image_stream_frame_rate(GatherSession *session, uint stream_id); /* Returns the frame rate of the stream. */
extern boolean	gather_session_image_stream_high_dynamic_range(GatherSession *session, uint stream_id); /* Returns TRUE if the image stream stores floats, and FALSE if the image stream stores uint8. */
extern uint8	*gather_session_image_stream_uint8_get(GatherSession *session, uint stream_id); /* Returns a pointer to uint8 RGBA data containing the image. */
extern float	*gather_session_image_stream_float_get(GatherSession *session, uint stream_id); /* Returns a pointer to float RGBA data containing the image. */
extern void		gather_session_image_stream_frame_advance(GatherSession *session, uint stream_id); /* Reading the current frame has been completed, and requesting a new frame.*/
extern double	gather_session_image_stream_frame_start(GatherSession *session, uint stream_id); /* Time when the frame should first be displayed*/


/* ---- Audio -----
Retrive audio data from a gather session. */

extern uint		gather_session_audio_stream_count_get(GatherSession *session); /* Returns the number of audio streams contained in the file. */
extern void		gather_session_audio_stream_active(GatherSession *session, uint stream_id, boolean active); /* activate or disable the stream so that its readable. */
extern boolean	gather_session_audio_stream_high_dynamic_range(GatherSession *session, uint stream_id); /* Returns TRUE if the audio stream stores floats, and FALSE if the audio stream stores int16. */
extern uint		gather_session_audio_stream_channel_count_get(GatherSession *session, uint stream_id); /* Returns the number of channels contained in a audio stream. */
extern void		*gather_session_audio_stream_channel_float_get(GatherSession *session, uint *length, uint stream_id, uint channel); /* Get the data form one of the channels in the stream. */
extern void		*gather_session_audio_stream_channel_int16_get(GatherSession *session, uint *length, uint stream_id, uint channel); /* Get the data form one of the channels in the stream. */
extern uint		gather_session_audio_stream_sample_rate_get(GatherSession *session, uint stream_id); /* Returns the sample rate per second of the audio data. */
extern void		gather_session_audio_stream_frame_advance(GatherSession *session, uint stream_id); /* Reading the current frame has been completed, and requesting a new frame.*/
extern double	gather_session_audio_stream_frame_start(GatherSession *session, uint stream_id); /* Time when the frame should first be displayed*/

/* ---- Utilities -----
Practical funtions to load a 2D image file. */

void *gather_image_load(char *file_name, uint *x, uint *y, boolean *hdr); /* Loads a RGBA Image form a file. */



/*

Get length.
Seek.

packing session create
unpacking session create
Save session create

packing session feed get
packing data get;
unpacking session feed data

Utility load/save image
utillity load save sound

*/

#endif
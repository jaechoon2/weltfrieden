extern int server_init(void);
extern void osc_send_pitch(float starttime, unsigned int chunk, 
			   float pitch, float flux, float centroid);
extern void osc_send_play(double when, int lowchunk, float pitch, float flux, float centroid);

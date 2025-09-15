Myst III/IV music player system (built in Unreal Engine 5.6, uses Quartz plugin): 

This project is intended to emulate how music plays in both Myst III: Exile and Myst IV: Revelation. It is capable of playing both single, 
one-shot tracks (such as a theme or cinematic track) and ambient tracks (which are composed of many individual tracks).  

The basic elements this music player and key terms:

	Single track: 	
		-Themes 
		-Cinematics 
  
	Ambient track: 
		-Pad – A longer, underlying track that tends to be the theme of a given location. 
		-Decorator – Shorter tracks, usually separated out into types (i.e. perc, thud, mldy, etc.), that play concurrently with a pad. 


The following is the structure of the music player system and classes needed for functionality:

MusicDataAsset: 

A child of UDataAsset that dictates how a given music track should play. This governs how music will play at a given location, not per 
age (i.e. J’nanin Observatory, not J’nanin as a whole). Single and ambient music tracks can both live in the same data asset concurrently, 
i.e. when linking to Amateria for the first time, the theme will play. Once the theme has finished, the ambient version of the theme will
start. This can be accomplished by loading a MusicDataAsset with the theme music track and having the next track be the ambient version. 
Specific elements of each described below. 


Single track options: 

	-Track name – Each single track must have a unique name for tracking purposes. 
	-Track – Specify a track to play (i.e. TOHB_Endgame_Music). 
   	-Volume - Volume multiplier for this specific track.
	-Fade settings - Fade in/out durations, if applicable. 
	-Next track – Specify the next track to play. Can be a single track or ambient track. When this track has completed, it will 
   					play this track next if it exists in the same data asset.


Ambient track options: 

	-Track Name – Like single tracks, ambient tracks require a name for tracking and next track purposes. 
	-Pads: 
		-Pad name – Each pad must have a unique pad name. 
		-Track – Specify a pad. 
  		-Volume - Volume multiplier for this specific track.
		-Should play in order - Whether the array of decorators should play in order or be played in a random order, true by default.
 		-Prohibited decorators - Decorators you do not want to play while this pad is playing.
		-Loop settings – Specify whether the pad should loop, the minimum number of loops, and the maximum number of loops. A random 
	 				integer between min and max is selected during runtime. 
		-Fade settings - Fade in and out duration for this pad, if applicable.
	-Beats Per Minute – Specify the beats per minute so that the Quartz clock is timed correctly with the pad and decorators.
	-Decorators: -
 			-Decorator Name - Specify a name for the decorator (must be unique).
			-Decorator - An array of decorators.
   			-Volume - A volume multiplier for the array of decorators. 
	  		-Should play in order - Whether the array of decorators should play in order, true by default.
	 		-Soundmixes - An array of soundmixes that will be applied while the  decorator is playing. 
			-Prohibited decorators - Other decorators you do not want to play while this decorator is playing, i.e. You don’t want 
   					drum_loop2 to play while drum_loop plays.
   			-Loops settings (previous defined) 
	  		-Decorator out - An optional Decorator Out (some looping decorators have a track that plays after a given number of loops),
	 		-Fade settings (specifically for this decorator)
	-Fade Settings – Specify the fade duration for the ambient track as a whole, how long it will fade in when it starts to play and fade out 
  					duration when it finally stops.
	-Starting delay for pad - How many beats after the Quartz clock starts before the first pad will play.
 	-Starting delay for primary decorator - How many beats after the Quartz clock starts before the primary decorator will play.
	-Starting delay for secondary decorator - How many beats after the Quartz clock starts before the secondary decorator will play.
 	-Frequency settings - An array of ten structs, each of which has four fields - Min delay before next pad, max delay before next pad, 
  					min delay before next decorator, max delay before next decorator. These ten structs coincide with the ten slider
					options for music frequency (0-9) and will determine how often pads and decorators in an ambient track will play.
	 				When an ambient track plays, a slider will appear in the lower right-hand corner of the screen. This would ideally 
	  				be inside of an in-game menu, but this is for sample/test purposes. 


BP_MusicPlayerComponent: 

  Ensure that a single instance of this component is added to your player character. This component will create and manage single and 
  ambient track components, attached to itself, when interacting with BP_MusicTrackTrigger that you can place in the game world.  

 
BP_SingleMusicTrackComponent: 

  Created by the MusicPlayerComponent and encapsulates all logic for single music track to play when the player interacts with a given 
  music track trigger. 


BP_AmbientMusicTrackComponent: 

Created by the MusicPlayerComponent and encapsulates all logic pertaining to ambient music tracks. Plays a specific ambient music 
track when the player character interacts with a given music track trigger. Three additional components can be created by this component 
which include a pad, primary decorator, and secondary decorator. Each of these components is an instance of DecoratorComponent. 
The blueprint creates, sets up, and starts a Quartz timer for accurate timing of decorator tracks to play. 
Reporting of ambient track info is done on each beat of the Quartz clock, which specifies: 

	  -The frequency (how often) pads and decorators should play.	 
	  -The number of beats until each of the decorator components will play next. 
		-The current track for each of the decorator components, or if it is waiting for a new 	one. 
		-A list of available (valid) pads. 
  	  -The number of beats for each decorator component that have elapsed since playing. 
	  -A list of available (valid) decorators, and how many decorators remain under each type.  


DecoratorComponent: 

  Governs playing and handling of a given decorator track, when and how many times it loops, and playing the Decorator Out track 
  if one is specified. Created by and attached to the AmbientMusicTrackComponent. 


BP_MusicTrackTrigger: 

  A blueprint to be placed in the game world in order to play a specific track. Specify the Track Name to play and the MusicDataAsset 
  it is associated with. Alternatively, you can check two different boxes under the Clear subcategory – Clear Track, which 
  will clear the track specified in Track Name, and Clear All Tracks, which will clear any track currently playing regardless of Track Name. 


Demonstrations of functionality:
Tomahna Amient	 			  							https://www.youtube.com/watch?v=KKJLdQ0avTM
J'Nanin Observatory Ambient 							https://www.youtube.com/watch?v=EOWRv5-ZSgg
Amateria Theme Ambient									https://www.youtube.com/watch?v=KQq35AmP8TU
Edanna Deadwood Ridge Ambient							https://www.youtube.com/watch?v=3M73JM7Gk38
J'Nanin Island Ambient with Myst III: Exile visuals 	https://www.youtube.com/watch?v=YtZYDJR5PEA









  

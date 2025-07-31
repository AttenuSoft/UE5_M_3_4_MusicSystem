Myst III/IV music player system (built in Unreal Engine 5.6, uses Quartz plugin): 

This project is intended to emulate how music plays in both Myst III: Exile and Myst IV: Revelation. It is capable of playing both single, 
one-shot tracks (such as a theme or cinematic track) and ambient tracks (which are composed of many individual tracks).  


Single track: 

	-Themes 
	-Cinematics 


Ambient track: 

	-Pad – A longer, underlying track that tends to be the theme of a given location. 
	-Decorator – Shorter tracks, usually separated out into types (i.e. perc, thud, mldy, 	etc.), that play concurrently with a pad. 


MusicDataAsset: 

A child of UDataAsset that dictates how a given music track should play. This governs how music will play at a given location, not per 
age (i.e. J’nanin Observatory, not J’anin as a whole).  


Single track options: 

	  -Track Name – Each single track must have a unique name for tracking purposes. 
	  -Track – Specify a track to play (i.e. TOHB_Endgame_Music). 
	  -Next Track – Specify the next track to play. Can be a single track or ambient track. When this track has completed, it will play this track next if it exists in the same data asset.


Ambient track options: 

	-Track Name – Like single tracks, ambient tracks require a name for tracking and next track purposes. 
	-Pads: 
		-Pad Name – Each pad must have a unique pad name. 
		-Track – Specify a pad. 
		-Loop settings – Specify whether the pad should loop, the minimum number of loops, and the maximum number of loops. A random 
	 	integer between min and max is selected during runtime. 
		-Beats Per Minute – Specify the beats per minute so that the Quartz clock is time correctly with the pad. 
	-Decorators – Specify an array of decorators, prohibited decorators (other decorators you do not want to play while this decorator 
 	is playing, i.e. You don’t want drum_loop2 to play while drum_loop plays), loops settings (previous defined), and an optional Decorator 
  	Out (some looping decorators have a track that plays after a 	given number of loops). 
	Fade Settings – Specify whether the ambient track as a whole should fade in and/or fade out and the duration of the fade in and fade out.  


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
You can manually set CurrentMusicFrequency, which is temporarily set on Begin Play, which will define how many beats to wait before 
playing another pad or decorator. Must be between 0 and 9, 0 being minimal music, 9 dictating that music should play most often.  
Reporting of ambient track info is done on each beat of the Quartz clock, which specifies: 

	  -The frequency (how often) pads and decorators should play.	 
	  -The number of beats until each of the decorator components will play next. 
		-The current track for each of the decorator components, or if it is waiting for a new 	one. 
		-A list of available (valid) pads. 
	  -A list of available (valid) decorators, and how many decorators remain under each type.  


DecoratorComponent: 

  Governs playing and handling of a given decorator track, when and how many times it loops, and playing the Decorator Out track 
  if one is specified. Created by and attached to the AmbientMusicTrackComponent. 


BP_MusicTrackTrigger: 

  A blueprint to be placed in the game world in order to play a specific track. Specify the Track Name to play and the Music Data 
  Asset it is associated with. Alternatively, you can check two different boxes under the Clear subcategory – Clear Track, which 
  will clear the track specified in Track Name, and Clear All Tracks, which will clear any track currently playing regardless of Track Name. 





  

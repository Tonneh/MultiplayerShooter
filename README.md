# MultiplayerShooter
MultiplayerShooter that is developed fully in Unreal Engine 5 C++. 


Includes a plugin to connect players to the same session, requires a steam login to host and connect to servers.  

Includes a Snowy Map 

Announcements for who won or which team has won, when the match starts, when the match ends, and when next match starts. 

A HUD that shows the players kill, deaths, ammo, time left in match, health, etc. 

Crouching, reloading, shooting, etc. 

3 game modes, Free for All, Teams, and Capture the Flag. 
- Free for All game mode will just keep track of the highest scoring player and send an announcement at the end displaying their name and their number of kills. 
Teams keep track of two teams score and announce which team has the most kills at the end as the winner. The characters mesh color will also change to blue and red, depending on their team. 
Capture the Flag will have a flag on teams side, whoever brings back the most flags wins. The flag carrier will be burdened and unable to fire. 

Lag Compensation for laggy players. Such as, ServerSideRewind (limited to 200 ping) and client-side prediction. 
- Server Side Rewind, the game will keep track of each players frame history, whenever a hit is sent from the client to the server, the server will rewind the time to when the player was hit (accounting for the amount of time it took for client to send to server) then confirming the hit then applying damage. Due to issues such as the being shot behind a wall problem, ServerSideRewind is limited to 200 ping. 
- Client-Side Prediction, for the ammo, aiming, and reloading. For ammo, its so the ammo in the HUD updates instantly and doesn't need to wait for the server to let the client know its ammo. For aiming and reloading, it's so the animations play correctly on the client. 

Developed with Unreal Engine 5


Weapon Types:
- Shotgun
- SMG
- Assault Rifle (HitScan/Projectile)
- Sniper Rifle (HitScan/Projectile) 
- Rocket Launcher
- Grenade Launcher 
- Pistol 
 
 Power Ups: 
 - Ammo Pickups
 - Health Packs
 - Shield Packs
 - Speed Packs
 - Jump Packs
 
 

#ifndef SERVER_CONTROL_H
#define SERVER_CONTROL_H

void ShutdownServer();
void LaunchServer(unsigned int server_port, Game::LevelType level_type);

#endif // SERVER_CONTROL
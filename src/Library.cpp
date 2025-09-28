#include "Library.h"
#include "Player.h"
#include "Crate.h"
#include "Enemy.h"

Library::Library() {
    map["Player"] = Player::create;
    map["Crate"] = Crate::create;
    map["Enemy"] = Enemy::create;
}
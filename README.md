# EQ2Emu – Linux Installation & Setup Guide

This guide walks you through compiling, installing, and running an **EverQuest II Emulator** server on Linux.

---

## 🛠️ Pre-Requisites

- **MariaDB Server** (MySQL is **not supported**)
- GCC 8 or newer (required for compilation)

> Ensure MariaDB is installed and running before continuing.

---

## ⚙️ Compilation & Installation

1. **Download the Linux compile script:**
   ```bash
   wget https://raw.githubusercontent.com/emagi/eq2emu/main/linux_compile.sh
   ```

2. **Edit configuration (optional):**
   - `LOG_FILE` – Path for build logs (defaults to current directory)
   - `EQ2EMU_HOME_DIR` – Base installation path (defaults to `/home/eq2emu`)

3. **Run the script:**
   ```bash
   bash -x linux_compile.sh
   ```

This will build and install all required binaries.

---

## 🗂️ Populating the Database

The server will not start until the game database is populated.

1. **Get the latest SQL dumps** for:
   - World database (`eq2emu.sql`)
   - Login database (`eq2emuls.sql`)

2. **Create two databases:**
   ```sql
   CREATE DATABASE eq2emu;
   CREATE DATABASE eq2ls;
   ```

3. **Import SQL data:**
   ```sql
   USE eq2emu;
   SOURCE LOCATIONOFSQL_UPDATES/eq2emu.sql;

   USE eq2ls;
   SOURCE LOCATIONOFSQL_UPDATES/eq2emuls.sql;
   ```

4. **Create a dedicated user:**
   ```sql
   CREATE USER 'eq2emu'@localhost IDENTIFIED BY 'eq2emu';
   GRANT ALL PRIVILEGES ON *.* TO 'eq2emu'@localhost IDENTIFIED BY 'eq2emu';
   ```

---

## 🚀 Preparing for First Run

Inside:  
`/home/eq2emu/eq2emu/server/`

### 1️⃣ Create `login_db.ini` (from `login_db.ini.example`)

```ini
[Database]
host=localhost
user=eq2emu         # your DB user
password=eq2emu     # your DB password
database=eq2ls
```

### 2️⃣ Create `world_db.ini` (from `world_db.ini.example`)

```ini
[Database]
host=127.0.0.1
user=eq2emu         # your DB user
password=eq2emu     # your DB password
database=eq2emu
```

### 3️⃣ Create `server_config.json` (from `server_config.json.example`)

```json
{
  "LoginServer": {
    "loginserver": "127.0.0.1",
    "loginport": "9100",
    "worldname": "TestLabs",
    "worldaddress": "127.0.0.1",
    "internalworldaddress": "127.0.0.1",
    "worldport": "9001",
    "locked": "false",
    "account": "testlabs",
    "password": "testpass"
  },
  "WorldServer": {
    "Defaultstatus": "0",
    "webaddress": "",
    "webport": "",
    "webcertfile": "",
    "webkeyfile": "",
    "webhardcodeuser": "",
    "webhardcodepassword": ""
  },
  "LoginConfig": {
    "ServerMode": "StandAlone",
    "ServerPort": "9100",
    "AccountCreation": "1",
    "webloginaddress": "",
    "webloginport": "",
    "webcertfile": "",
    "webkeyfile": "",
    "webhardcodeuser": "",
    "webhardcodepassword": ""
  }
}
```

Replace:
- **loginserver** – IP of login server (e.g. `127.0.0.1`)
- **worldname** – Your in-game server name
- **worldaddress/internalworldaddress** – IP of world server
- **account/password** – Server account credentials

### 4️⃣ Create `log_config.xml` (from `log_config.xml.example`)

### 5️⃣ Add Server to the Login Database

```sql
USE eq2ls;
INSERT INTO login_worldservers
SET note='',
    description='',
    name='$WORLD_NAME',
    account='$WORLD_ACCOUNT_NAME',
    password=SHA2('$WORLD_ACCOUNT_PASSWORD',512);
```

Replace the variables:
- `$WORLD_NAME` – Your chosen server name  
- `$WORLD_ACCOUNT_NAME` – `account` value from `server_config.json`  
- `$WORLD_ACCOUNT_PASSWORD` – `password` value from `server_config.json`

---

## ▶️ Running the Server

Both executables must run:

```bash
./login &
./eq2world &
```

Or using `screen`:

```bash
screen -d -m ./login
screen -d -m ./eq2world
```

---

## 🔄 Keeping Up-to-Date

To stay current:

1. Update source code:
   ```bash
   git pull
   ```

2. Rebuild and copy binaries to your server location.

3. Apply the latest SQL updates.

**Optional automation script:**
```bash
wget -cO - https://www.dropbox.com/s/32qqmmviblulhwl/updater.sh?dl=1 > updater.sh
chmod +x updater.sh
./updater.sh
```

Then apply new SQL files found in `SQL_UPDATES`.

---

## 🧩 Linux Troubleshooting

| Issue | Solution |
|-------|---------|
| `cc1plus: error: -Werror=class-memaccess` | Use GCC 8 or newer (`gcc -v` to check). |
| `mysql.h` missing | Install `libmariadb-dev` or edit Makefile: `-I/usr/include/mysql` in `COPTS=$(WFLAGS)`. |
| `lmariadbclient` missing | In Makefile, replace `-lmariadbclient` with `-lmysqlclient`. |
| `undefined reference to WorldDatabase::LoadRuleSets()` | Run `make clean` then `make` again. |
| `fmt/format.h` not found | Edit `EQ2EMu/EQ2/source/common/string_util.h`: change `#include <fmt/format.h>` → `#include <../depends/fmt/include/fmt/format.h>`. |

---

## ✅ Summary

- **Compile** with the provided script  
- **Populate** both `eq2emu` and `eq2ls` databases  
- **Configure** the `.ini` and `.json` files  
- **Run** `./login` and `./eq2world`  

Your EQ2Emu server should now be up and running on Linux.
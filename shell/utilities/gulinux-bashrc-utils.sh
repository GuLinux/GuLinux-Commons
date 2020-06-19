BASHRC_FILE="${BASHRC_FILE:-$HOME/.bashrc}"
alias rebashrc="source $BASHRC_FILE"

function vimbashrc() {
    vim "${BASHRC_FILE}"
    rebashrc
}

function vimbashrc-gulinux() {
    vim "${BASH_SOURCE}"
    rebashrc
}

function update-gulinux-commons() {
    (
        cd "$( dirname "${BASH_SOURCE}" )"
        cd "$( git rev-parse --show-toplevel )"
        git pull
    )
    rebashrc
}

export GULINUX_BASH_UTILS="$( cd "$( dirname "$BASH_SOURCE" )"; pwd )"
export PATH="${GULINUX_BASH_UTILS}:$PATH"
export EDITOR=vim
export VISUAL=vim
function mkcd() {
    mkdir -p "$@"
    cd "$@"
}

function mkdatedir() {
    if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
        cat <<EOF
Usage: mkdatedir [-t|-u] [-s <suffix>] [cd]

Options:
  -t          : append time to directory name (default: date only)
  -u          : use UTC timestamp instead of formatted date
  -s <suffix> : append the specified suffix to the directory name
  cd          : change directory after creating it
EOF
        return
    fi
    SUFFIX=""
    DATESPEC="%Y-%m-%d"
    if [ "$1" = '-t' ]; then
        DATESPEC="${DATESPEC}T%H-%M-%S"
        shift
    elif [ "$1" = '-u' ]; then
        DATESPEC="%s"
        shift
    fi
    if [ "$1" = "-s" ]; then
        SUFFIX="$2"
        shift
        shift
    fi
    dirname="$( ${DATE_COMMAND:-date} +${DATESPEC} )$SUFFIX"
    mkdir -p "$dirname"
    if [ "$1" = "cd" ]; then
        cd "$dirname"
    fi
}

if [ "$SKIP_BASH_PROMPT" != "true" ]; then
    has_git_dir() {
        CURDIR="$PWD"
        while [ -d "$CURDIR" ]  && [ "$CURDIR" != "/" ]; do
            [ -d "$CURDIR/.git" ] && return 0
            CURDIR="$( cd "$CURDIR/.."; pwd )"
        done
        return 1
    }

    # Custom bash prompt
    # get current branch in git repo
    function parse_git_branch() {
        #has_git_dir && return 0
        if ! has_git_dir; then
            return 0
        fi
        BRANCH=`git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/'`
        if [ ! "${BRANCH}" = "" ]
        then
            STAT=`parse_git_dirty`
            echo "[${BRANCH}${STAT}]"
        else
            echo ""
        fi
    }

    # get current status of git repo
    # http://ezprompt.net/
    function parse_git_dirty {
        status=`git status 2>&1 | tee`
        dirty=`echo -n "${status}" 2> /dev/null | grep "modified:" &> /dev/null; echo "$?"`
        untracked=`echo -n "${status}" 2> /dev/null | grep "Untracked files" &> /dev/null; echo "$?"`
        ahead=`echo -n "${status}" 2> /dev/null | grep "Your branch is ahead of" &> /dev/null; echo "$?"`
        newfile=`echo -n "${status}" 2> /dev/null | grep "new file:" &> /dev/null; echo "$?"`
        renamed=`echo -n "${status}" 2> /dev/null | grep "renamed:" &> /dev/null; echo "$?"`
        deleted=`echo -n "${status}" 2> /dev/null | grep "deleted:" &> /dev/null; echo "$?"`
        bits=''
        if [ "${renamed}" = "0" ]; then
            bits=">${bits}"
        fi
        if [ "${ahead}" = "0" ]; then
            bits="*${bits}"
        fi
        if [ "${newfile}" = "0" ]; then
            bits="+${bits}"
        fi
        if [ "${untracked}" = "0" ]; then
            bits="?${bits}"
        fi
        if [ "${deleted}" = "0" ]; then
            bits="x${bits}"
        fi
        if [ "${dirty}" = "0" ]; then
            bits="!${bits}"
        fi
        if [ ! "${bits}" = "" ]; then
            echo " ${bits}"
        else
            echo ""
        fi
    }

    export PS1="\[\e[33m\]\u\[\e[m\]@\[\e[94m\]\h\[\e[m\] \[\e[32m\]\w\[\e[m\] \[\e[31m\]\`parse_git_branch\`\[\e[m\] \\$ "
    # end custom bash prompt
fi

function tmux-session() {
    if [ "$1" = "--help" ]; then
        echo "Usage: tmux-session [SESSION_DIR] [SESSION_NAME]"
        exit 1
    fi
    SESSION_DIR="${1:-$PWD}"
    SESSION_DIR="$( cd "$SESSION_DIR"; pwd )"
    SESSION_NAME="${2:-$( basename "${SESSION_DIR}" )}"
    tmux new-session -As "$SESSION_NAME" -c "$SESSION_DIR"
}


function dotenv-source() {
    DIR="$PWD"
    while [ "$DIR" != "/" ]; do
        echo '[=] Searching for `.env` in ' "$DIR" >&2
        if [ -r "$DIR/.env" ]; then
            . "$DIR/.env"
            echo "[+] Found $DIR/.env" >&2
            break
        else
            DIR="$( dirname "$DIR" )"
        fi
    done
}


function install_bashrc() {
    BASHRC_FILE="${1:-$HOME/.bashrc}"
    SELF_FILE="$( cd "$( dirname "$BASH_SOURCE")"; pwd)/$(basename "$BASH_SOURCE")"
    cat >>"$BASHRC_FILE" <<EOF
export BASHRC_FILE="$BASHRC_FILE"
[ -r "$SELF_FILE" ] && source "$SELF_FILE"
EOF
}

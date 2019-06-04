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

export EDITOR=vim
export VISUAL=vim
function mkcd() {
    mkdir -p "$@"
    cd "$@"
}

function mkdatedir() {
    DATESPEC="%Y-%m-%d"
    if [ "$1" = '-t' ]; then
        DATESPEC="${DATESPEC}T%H-%M-%S"
        shift
    elif [ "$1" = '-u' ]; then
        DATESPEC="%s"
        shift
    fi
    dirname="$( ${DATE_COMMAND:-date} +${DATESPEC} )"
    mkdir -p "$dirname"
    if [ "$1" = "cd" ]; then
        cd "$dirname"
    fi
}

if [ "$SKIP_BASH_PROMPT" != "true" ]; then
    # Custom bash prompt
    # get current branch in git repo
    function parse_git_branch() {
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

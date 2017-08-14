function sync {
		if [[ -d $dir ]]; then
				source=$dir
				adjremote=1
		else
				source=$remote
		fi
		if [[ -d $dest ]]; then
				[[ -n "$nocheck" ]] && return
				cd $dest
				git pull local
				git pull origin
				cd ..
		else
				git clone $source $dest
				if [[ -n "$adjremote" ]]; then
						cd $dest
						git remote set-url origin $remote
						git remote add local $source
						cd ..
				fi
		fi
}
	
dir=/extra/home/packages/git/libxml2
remote=git://git.gnome.org/libxml2
dest=libxml2
sync

dir=/home/code/note
remote=https://github.com/cyisfor/note.git
dest=note
sync

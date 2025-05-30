((nil . ((eval . (defun hk/leetcodechef-copy (name)
                   (interactive "sEnter name: ")
                   (copy-directory "template" (denote-format-file-name "./" (denote-get-identifier (current-time)) nil name nil nil)))))))

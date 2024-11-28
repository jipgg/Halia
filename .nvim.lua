require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=Halia.d.luau",
    },
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
            },
        }
    }
}

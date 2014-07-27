package main

import "github.com/go-martini/martini"

func main() {
  m := martini.Classic()
  m.Get("/:name", func(params martini.Params) string {
    return "Hello " + params["name"]
  })

  m.Get("/", func() string {
    return "Hello world"
  })

  m.Run()
}
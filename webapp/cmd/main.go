package main

import (
	"html/template"
	"io"
	"net/http"
    "os"
    "fmt"
	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
)

type TemplateRegistry struct {
	templates *template.Template
}



func (t *TemplateRegistry) Render(w io.Writer,
                          name string,
                          data interface{}, c echo.Context) error {
	return t.templates.ExecuteTemplate(w, name, data)
}

func main() {
	// ECHO INSTANCE
	e := echo.New()

	// MIDDLEWARE
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())

    e.Renderer = &TemplateRegistry{
        templates: template.Must(template.ParseGlob("web/ui/views/*.html")),

    }

	// ROUTES
    e.GET("/", indexHandler)
    e.Static("/app", "web/js")
    e.Logger.Fatal(e.Start(fmt.Sprintf(":%s", os.Getenv("PORT"))))
}

func indexHandler(c echo.Context) error {
    fmt.Println("Index Handler")
    return c.Render(http.StatusOK, "index.html", nil)
}

